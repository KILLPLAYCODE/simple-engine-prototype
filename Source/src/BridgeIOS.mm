#import "BridgeIOS.h"
#import <UIKit/UIKit.h>
#import <UniformTypeIdentifiers/UniformTypeIdentifiers.h>

@interface DocumentPickerDelegate : NSObject <UIDocumentPickerDelegate>
@end

@implementation DocumentPickerDelegate

- (void)documentPicker:(UIDocumentPickerViewController *)controller didPickDocumentsAtURLs:(NSArray<NSURL *> *)urls {
    NSURL *selectedURL = [urls firstObject];
    if (!selectedURL) return;

    if ([selectedURL startAccessingSecurityScopedResource]) {
        
        NSArray *paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
        NSString *cacheDirectory = [paths objectAtIndex:0];
        NSString *fileName = [selectedURL lastPathComponent];
        NSString *destinationPath = [cacheDirectory stringByAppendingPathComponent:fileName];
        
        NSFileManager *fileManager = [NSFileManager defaultManager];
        NSError *error = nil;
        
        if ([fileManager fileExistsAtPath:destinationPath]) {
            [fileManager removeItemAtPath:destinationPath error:nil];
        }
        
        [fileManager copyItemAtPath:[selectedURL path] toPath:destinationPath error:&error];
        [selectedURL stopAccessingSecurityScopedResource];
        
        if (!error) {
            std::string cppPath = [destinationPath UTF8String];
            std::string cppFileName = [fileName UTF8String];
            IOSBridge::onFileSelected(cppPath,cppFileName);
        }
    }
}
@end

static DocumentPickerDelegate* pickerDelegate = nil;

void IOSBridge::importMediaFile(std::string type) {
    NSString* objectType = [NSString stringWithUTF8String:type.data()];
    NSArray<UTType *> *types = @[];
    if([objectType isEqualToString:@"audio"]){
        types = @[UTTypeAudio];
    } else if(objectType isEqualToString:@"image"){
        types = @[UTTypeImage];
    } else {
        types = @[UTTypeAudio, UTTypeImage];
    }
    
    UIDocumentPickerViewController *picker = [[UIDocumentPickerViewController alloc] initWithContentTypes:types asCopy:YES];
    
    if (!pickerDelegate) {
        pickerDelegate = [[DocumentPickerDelegate alloc] init];
    }
    picker.delegate = pickerDelegate;
    
    UIViewController *rootViewController = [UIApplication sharedApplication].keyWindow.rootViewController;
    [rootViewController presentViewController:picker animated:YES completion:nil];
}
