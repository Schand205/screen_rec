// capture.mm
#import <ScreenCaptureKit/ScreenCaptureKit.h>
#import <CoreImage/CoreImage.h>
#import <CoreMedia/CoreMedia.h>
#include "capture.h"
#include <dispatch/dispatch.h>

@interface Bridge : NSObject <SCStreamOutput>
@property dispatch_semaphore_t sem;
@property Screenshot result;
@end

@implementation Bridge
- (void)stream:(SCStream*)s
        didOutputSampleBuffer:(CMSampleBufferRef)buf
        ofType:(SCStreamOutputType)type {

    CVImageBufferRef ib = CMSampleBufferGetImageBuffer(buf);
    if (!ib) return;

    CVPixelBufferLockBaseAddress(ib, kCVPixelBufferLock_ReadOnly);

    size_t w   = CVPixelBufferGetWidth(ib);
    size_t h   = CVPixelBufferGetHeight(ib);
    size_t bpr = CVPixelBufferGetBytesPerRow(ib);
    void*  src = CVPixelBufferGetBaseAddress(ib);

    uint8_t* buf2 = new uint8_t[h * bpr];
    memcpy(buf2, src, h * bpr);

    CVPixelBufferUnlockBaseAddress(ib, kCVPixelBufferLock_ReadOnly);

    self.result = { buf2, (int)w, (int)h, (int)bpr };
    dispatch_semaphore_signal(self.sem);
}
@end

Screenshot takeScreenshot() {
    __block Screenshot out = {};
    dispatch_semaphore_t done = dispatch_semaphore_create(0);

    [SCShareableContent getShareableContentWithCompletionHandler:
        ^(SCShareableContent* c, NSError* e) {

        SCDisplay* d = c.displays.firstObject;
        SCContentFilter* f = [[SCContentFilter alloc]
            initWithDisplay:d excludingWindows:@[]];

        SCStreamConfiguration* cfg = [[SCStreamConfiguration alloc] init];
        cfg.width = d.width; cfg.height = d.height;
        cfg.pixelFormat = kCVPixelFormatType_32BGRA;

        Bridge* bridge = [[Bridge alloc] init];
        bridge.sem = dispatch_semaphore_create(0);

        SCStream* stream = [[SCStream alloc] initWithFilter:f
            configuration:cfg delegate:nil];
        [stream addStreamOutput:bridge type:SCStreamOutputTypeScreen
            sampleHandlerQueue:dispatch_get_global_queue(0,0) error:nil];

        [stream startCaptureWithCompletionHandler:^(NSError*_) {
            dispatch_semaphore_wait(bridge.sem,
                dispatch_time(DISPATCH_TIME_NOW, 3*NSEC_PER_SEC));
            out = bridge.result;
            [stream stopCaptureWithCompletionHandler:^(NSError*_) {
                dispatch_semaphore_signal(done);
            }];
        }];
    }];

    dispatch_semaphore_wait(done,
        dispatch_time(DISPATCH_TIME_NOW, 10*NSEC_PER_SEC));
    return out;
}

void freeScreenshot(Screenshot& s) {
    delete[] s.pixels;
    s.pixels = nullptr;
}
