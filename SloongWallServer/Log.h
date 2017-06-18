#pragma once
int WriteToLog(char * szFormat, ...);
int WriteToScreen(char *szFormat, ...);
void DumpBuffToFile(unsigned char* buf, int len);
void DumpBuffToScreen(unsigned char* buf, int len);
