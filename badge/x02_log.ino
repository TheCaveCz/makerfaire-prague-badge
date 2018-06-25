//////////////////////////////
//
// MakerFaire Prague 2018 badge
// (c) The Cave, 2018
// https://thecave.cz
//

//
// Logging macros
//

#if LOG_ENABLED
#define logRaw(msg) Serial.print(msg)
#define logNum(msg,c) Serial.print(msg,c)
#define logFlush() Serial.flush()
#else
#define logRaw(msg)
#define logNum(msg,c)
#define logFlush()
#endif

#define logLine() logRaw('\n')
#define logHeader() logRaw(millis());logRaw(" | ");
#define logInfo(msg) logHeader();logRaw(F(msg));logLine()
#define logValue(msg,val) logHeader();logRaw(F(msg));logRaw(val);logLine()

