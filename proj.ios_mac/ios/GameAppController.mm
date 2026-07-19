#import "GameAppController.h"
#import "GameViewController.h"
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>

static void handleCrashSignal(int sig) {
    void *callstack[128];
    int frames = backtrace(callstack, 128);
    char **strs = backtrace_symbols(callstack, frames);

    NSMutableString *log = [NSMutableString stringWithFormat:@"Crash signal: %d\n", sig];
    for (int i = 0; i < frames; i++) {
        [log appendFormat:@"%s\n", strs[i]];
    }
    free(strs);

    NSString *docsPath = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) firstObject];
    NSString *logPath = [docsPath stringByAppendingPathComponent:@"crash_log.txt"];
    [log writeToFile:logPath atomically:YES encoding:NSUTF8StringEncoding error:nil];

    signal(sig, SIG_DFL);
    raise(sig);
}

static void installCrashHandler() {
    signal(SIGABRT, handleCrashSignal);
    signal(SIGILL, handleCrashSignal);
    signal(SIGSEGV, handleCrashSignal);
    signal(SIGBUS, handleCrashSignal);
    signal(SIGFPE, handleCrashSignal);
    signal(SIGTRAP, handleCrashSignal);
}

@implementation GameAppController

#pragma mark -
#pragma mark Application lifecycle

- createRootViewController {
    installCrashHandler();

    GameViewController* viewController = [[GameViewController alloc] init];
    return viewController;
}

@end