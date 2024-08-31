#ifndef MUHLE_INTELLIGENCE_H
#define MUHLE_INTELLIGENCE_H

#ifdef __cplusplus
extern "C" {
#endif

#define MUHLE_INTELLIGENCE_ERROR -1
#define MUHLE_INTELLIGENCE_SUCCESS 0
#define MUHLE_INTELLIGENCE_MESSAGE_UNAVAILABLE 1

int muhle_intelligence_initialize();
int muhle_intelligence_uninitialize();
int muhle_intelligence_send(const char* string);
int muhle_intelligence_receive_size(unsigned int* size);
int muhle_intelligence_receive(char* string);

#ifdef __cplusplus
}
#endif

#endif
