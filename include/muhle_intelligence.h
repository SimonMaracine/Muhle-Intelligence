#ifndef MUHLE_INTELLIGENCE_H
#define MUHLE_INTELLIGENCE_H

#ifdef __cplusplus
extern "C" {
#endif

#define MUHLE_INTELLIGENCE_ERROR -1
#define MUHLE_INTELLIGENCE_SUCCESS 0
#define MUHLE_INTELLIGENCE_MESSAGE_UNAVAILABLE 1

extern int muhle_intelligence_initialize();
extern int muhle_intelligence_uninitialize();
extern int muhle_intelligence_send(const char* string);
extern int muhle_intelligence_receive_size(unsigned int* size);
extern int muhle_intelligence_receive(char* string);

#ifdef __cplusplus
}
#endif

#endif
