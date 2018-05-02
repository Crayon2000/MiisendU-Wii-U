/****************************************************************************
 * Copyright (C) 2016 Dimok
 * Modified by Maschell, 2018
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/
#include <malloc.h>
#include <string.h>
#include "FileDownloader.h"
#include <dynamic_libs/curl_functions.h>
#include "utils/logger.h"
#include "utils/utils.h"


bool FileDownloader::getFile(const std::string & downloadUrl, std::string & fileBuffer, ProgressCallback callback, void *arg) {
    curl_private_data_t private_data;
    private_data.progressCallback = callback;
    private_data.progressArg = arg;
    private_data.buffer = 0;
    private_data.filesize = 0;
    private_data.file = 0;

    bool result = internalGetFile(downloadUrl, &private_data);

    if(private_data.filesize > 0 && private_data.buffer) {
        fileBuffer.resize(private_data.filesize);
        memcpy(&fileBuffer[0], private_data.buffer, private_data.filesize);
    }

    if(private_data.buffer) {
        free(private_data.buffer);
    }

    return result;
}

bool FileDownloader::getFile(const std::string & downloadUrl, const std::string & outputPath, ProgressCallback callback, void *arg) {
    curl_private_data_t private_data;
    private_data.progressCallback = callback;
    private_data.progressArg = arg;
    private_data.buffer = 0;
    private_data.filesize = 0;

    s32 res = open(outputPath.c_str(), O_CREAT | O_TRUNC | O_WRONLY);
    close(res);

    private_data.file = new CFile(outputPath.c_str(), CFile::WriteOnly);

    if(!private_data.file->isOpen()) {
        delete private_data.file;
        DEBUG_FUNCTION_LINE("Can not write to file %s\n", outputPath.c_str());
        return false;
    }

    bool result = internalGetFile(downloadUrl, &private_data);

    private_data.file->close();
    delete private_data.file;
    return result;
}

bool FileDownloader::internalGetFile(const std::string & downloadUrl, curl_private_data_t * private_data) {
    bool result = false;
    int resp = 404;
    int ret = -1;
    CURL * curl = n_curl_easy_init();
    if(!curl) {
        return false;
    }

    int ssl_context = -1;

    std::string prefix = "https";
    if(strncmp(downloadUrl.c_str(), prefix.c_str(), prefix.size()) == 0) {
        DEBUG_FUNCTION_LINE("Needs a SSL context\n");
        ssl_context = NSSLCreateContext(0);
        if(ssl_context < 0) {
            DEBUG_FUNCTION_LINE("Failed to create SSL Context\n");
            goto exit_error;
        }

        // Add all existing certs
        for(int i = 100; i<106; i++) {
            NSSLAddServerPKI(ssl_context,i);
        }

        for(int i = 1001; i<1034; i++) {
            NSSLAddServerPKI(ssl_context,i);
        }
        n_curl_easy_setopt(curl, CURLOPT_GSSAPI_DELEGATION, ssl_context); // Is CURLOPT_NSSL_CONTEXT on the Wii U
    }

    n_curl_easy_setopt(curl, CURLOPT_URL, downloadUrl.c_str());
    n_curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, FileDownloader::curlCallback);
    n_curl_easy_setopt(curl, CURLOPT_WRITEDATA, private_data);
    n_curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    //n_curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuf);

    if(private_data->progressCallback) {
        n_curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, FileDownloader::curlProgressCallback);
        n_curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, private_data);
        n_curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
    }

    ret = n_curl_easy_perform(curl);
    if(ret) {
        DEBUG_FUNCTION_LINE("n_curl_easy_perform ret %i\n", ret);
        goto exit_error;
    }

    if(!private_data->filesize) {
        DEBUG_FUNCTION_LINE("file length is 0\n");
        goto exit_error;
    }

    n_curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &resp);

    if(resp != 200) {
        DEBUG_FUNCTION_LINE("response != 200\n");
        goto exit_error;
    }

    result = true;

exit_error:
    if(ssl_context >= 0) {
        NSSLDestroyContext(ssl_context);
    }

    n_curl_easy_cleanup(curl);
    return result;
}

int FileDownloader::curlCallback(void *buffer, int size, int nmemb, void *userp) {
    curl_private_data_t *private_data = (curl_private_data_t *)userp;
    int read_len = size*nmemb;

    if(private_data->file) {
        int res = private_data->file->write((u8*)buffer, read_len);
        private_data->filesize += res;
        return res;
    } else {
        if(!private_data->buffer) {
            private_data->buffer = (u8*) malloc(read_len);
        } else {
            u8 *tmp = (u8*) realloc(private_data->buffer, private_data->filesize + read_len);
            if(!tmp) {
                free(private_data->buffer);
                private_data->buffer = NULL;
            } else {
                private_data->buffer = tmp;
            }
        }

        if(!private_data->buffer) {
            private_data->filesize = 0;
            return -1;
        }

        memcpy(private_data->buffer + private_data->filesize, buffer, read_len);
        private_data->filesize += read_len;
        return read_len;
    }
}

int FileDownloader::curlProgressCallback(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow) {
    curl_private_data_t *private_data = (curl_private_data_t *)clientp;
    if(private_data->progressCallback) {
        private_data->progressCallback(private_data->progressArg, (u32)dlnow, (u32)dltotal);
    }
    return 0;
}

