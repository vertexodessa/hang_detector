#ifndef HANGDETECTOR_H
#define HANGDETECTOR_H

class HangAction {
 HangAction(int delay, int threadId)
 HangAction(int delay, function<void(void*)> callback, void *userData)
}

class HangDetector {
 HangDetector()
 start(int interval =1000)
 restart()
 addAction(HangAction)
 clearActions()
private:
 static void initialize();
 static void shutdown();
}

#endif