#ifndef MUHLE_INTELLI_H
#define MUHLE_INTELLI_H

/*
    Muhle Intelligence C API, an alternative interface to the engine.
    Calls are not thread-safe, as they access global data.
*/

#define MUHLE_INTELLIGENCE_ERROR -1
#define MUHLE_INTELLIGENCE_SUCCESS 0
#define MUHLE_INTELLIGENCE_MESSAGE_UNAVAILABLE 1

#ifdef __cplusplus
extern "C" {
#endif

/*
    Initialize the engine context.

    Returns MUHLE_INTELLIGENCE_ERROR, if the context is already initialized, or if the initialization fails,
    otherwise MUHLE_INTELLIGENCE_SUCCESS.
*/
int muhle_intelligence_initialize();

/*
    Uninitialize the engine context.

    Returns MUHLE_INTELLIGENCE_ERROR, if the context is not initialized, otherwise MUHLE_INTELLIGENCE_SUCCESS.
*/
int muhle_intelligence_uninitialize();

/*
    Send a message to the engine. The string must be null-terminated (C string).

    Returns MUHLE_INTELLIGENCE_ERROR, if the context is not initialized, or if the engine fails to execute the
    command, otherwise MUHLE_INTELLIGENCE_SUCCESS.
*/
int muhle_intelligence_send(const char* string);

/*
    Receive the next message's size from the engine. If there is no buffered message, then 0 will be written into
    the pointer.

    Returns MUHLE_INTELLIGENCE_ERROR, if the context is not initialized, or if an error occurrs.
    Returns MUHLE_INTELLIGENCE_MESSAGE_UNAVAILABLE, if there is no buffered message, otherwise
    MUHLE_INTELLIGENCE_SUCCESS.
*/
int muhle_intelligence_receive_size(unsigned int* size);

/*
    Receive the next message's contents from the engine. If there is no buffered message, then nothing will be
    written into the pointer, otherwise N bytes will be written, where N is the size received on the previous
    successful muhle_intelligence_receive_size call.

    You must call muhle_intelligence_receive once only when muhle_intelligence_receive_size returns
    MUHLE_INTELLIGENCE_SUCCESS.

    Returns MUHLE_INTELLIGENCE_ERROR, if the context is not initialized, if an error occurrs, or if there is no
    buffered message. Otherwise returns MUHLE_INTELLIGENCE_SUCCESS.

    Example usage:

    char* receive_next_engine_message() {
        int result;

        unsigned int size;
        result = muhle_intelligence_receive_size(&size);

        if (result == MUHLE_INTELLIGENCE_ERROR) {
            // ...
            return NULL;
        }

        if (result == MUHLE_INTELLIGENCE_MESSAGE_UNAVAILABLE) {
            return NULL;
        }

        char* buffer = malloc((size_t) size);

        if (buffer == NULL) {
            // ...
            return NULL;
        }

        result = muhle_intelligence_receive(buffer);

        if (result == MUHLE_INTELLIGENCE_ERROR) {
            // ...
            return NULL;
        }

        return buffer;
    }
*/
int muhle_intelligence_receive(char* string);

#ifdef __cplusplus
}
#endif

#endif
