#import "BridgeIOS.h"
#import <UIKit/UIKit.h>
#import <PhotosUI/PhotosUI.h>
#import <UniformTypeIdentifiers/UniformTypeIdentifiers.h>

@interface MediaPickerDelegate : NSObject <UIDocumentPickerDelegate, PHPickerViewControllerDelegate>
@end

@implementation MediaPickerDelegate

#pragma mark - PHPickerViewControllerDelegate (изображения)

- (void)picker:(PHPickerViewController *)picker didFinishPicking:(NSArray<PHPickerResult *> *)results {
    [picker dismissViewControllerAnimated:YES completion:nil];

    if (results.count == 0) {
        IOSBridge::onFileSelected("", "");
        return;
    }

    PHPickerResult *result = results.firstObject;
    NSItemProvider *provider = result.itemProvider;

    if (![provider hasItemConformingToTypeIdentifier:UTTypeImage.identifier]) {
        IOSBridge::onFileSelected("", "");
        return;
    }

    [provider loadFileRepresentationForTypeIdentifier:UTTypeImage.identifier
                                     completionHandler:^(NSURL *url, NSError *error) {
        if (!url || error) {
            NSLog(@"[Axmol Bridge] Ошибка получения изображения: %@", error.localizedDescription);
            dispatch_async(dispatch_get_main_queue(), ^{
                IOSBridge::onFileSelected("", "");
            });
            return;
        }

        NSArray *paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
        NSString *cacheDirectory = [paths objectAtIndex:0];

        NSString *extension = url.pathExtension.length > 0 ? url.pathExtension : @"jpg";
        NSString *fileName = [[[NSUUID UUID].UUIDString stringByAppendingPathExtension:extension] lastPathComponent];
        NSString *destinationPath = [cacheDirectory stringByAppendingPathComponent:fileName];

        [[NSFileManager defaultManager] removeItemAtPath:destinationPath error:nil];

        NSError *copyError = nil;
        BOOL success = [[NSFileManager defaultManager] copyItemAtURL:url
                                                                  toURL:[NSURL fileURLWithPath:destinationPath]
                                                                  error:&copyError];

        dispatch_async(dispatch_get_main_queue(), ^{
            if (success && !copyError) {
                std::string cppPath = [destinationPath UTF8String];
                std::string cppFileName = [fileName UTF8String];
                IOSBridge::onFileSelected(cppPath, cppFileName);
            } else {
                NSLog(@"[Axmol Bridge] Ошибка копирования изображения: %@", copyError.localizedDescription);
                IOSBridge::onFileSelected("", "");
            }
        });
    }];
}

#pragma mark - UIDocumentPickerDelegate (аудио / файлы)

- (void)documentPicker:(UIDocumentPickerViewController *)controller didPickDocumentsAtURLs:(NSArray<NSURL *> *)urls {
    [controller dismissViewControllerAnimated:YES completion:nil];

    NSURL *selectedURL = [urls firstObject];
    if (!selectedURL) {
        IOSBridge::onFileSelected("", "");
        return;
    }

    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        BOOL accessGranted = [selectedURL startAccessingSecurityScopedResource];

        NSError *downloadError = nil;
        [[NSFileManager defaultManager] startDownloadingUbiquitousItemAtURL:selectedURL error:&downloadError];

        NSError *coordinatorError = nil;
        NSFileCoordinator *coordinator = [[NSFileCoordinator alloc] init];

        [coordinator coordinateReadingItemAtURL:selectedURL options:NSFileCoordinatorReadingWithoutChanges error:&coordinatorError byAccessor:^(NSURL *newURL) {
            NSArray *paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
            NSString *cacheDirectory = [paths objectAtIndex:0];
            NSString *fileName = [newURL lastPathComponent];
            NSString *destinationPath = [cacheDirectory stringByAppendingPathComponent:fileName];

            NSError *readError = nil;
            NSData *fileData = [NSData dataWithContentsOfURL:newURL options:NSDataReadingMappedIfSafe error:&readError];

            if (fileData && !readError) {
                NSError *writeError = nil;
                BOOL success = [fileData writeToFile:destinationPath options:NSDataWritingAtomic error:&writeError];

                if (success && !writeError) {
                    std::string cppPath = [destinationPath UTF8String];
                    std::string cppFileName = [fileName UTF8String];
                    dispatch_async(dispatch_get_main_queue(), ^{
                        IOSBridge::onFileSelected(cppPath, cppFileName);
                    });
                } else {
                    NSLog(@"[Axmol Bridge] Ошибка записи файла в кэш: %@", writeError.localizedDescription);
                    dispatch_async(dispatch_get_main_queue(), ^{
                        IOSBridge::onFileSelected("", "");
                    });
                }
            } else {
                NSLog(@"[Axmol Bridge] Ошибка чтения оригинального файла: %@", readError.localizedDescription);
                dispatch_async(dispatch_get_main_queue(), ^{
                    IOSBridge::onFileSelected("", "");
                });
            }
        }];

        if (accessGranted) {
            [selectedURL stopAccessingSecurityScopedResource];
        }

        if (coordinatorError) {
            NSLog(@"[Axmol Bridge] Ошибка координатора файлов: %@", coordinatorError.localizedDescription);
            dispatch_async(dispatch_get_main_queue(), ^{
                IOSBridge::onFileSelected("", "");
            });
        }
    });
}

- (void)documentPickerWasCancelled:(UIDocumentPickerViewController *)controller {
    [controller dismissViewControllerAnimated:YES completion:nil];
    IOSBridge::onFileSelected("", "");
}

#pragma mark - Общие утилиты

+ (void)showAlertWithTitle:(NSString *)title message:(NSString *)message {
    dispatch_async(dispatch_get_main_queue(), ^{
        UIAlertController *alert = [UIAlertController alertControllerWithTitle:title
                                                                       message:message
                                                                preferredStyle:UIAlertControllerStyleAlert];

        UIAlertAction *okAction = [UIAlertAction actionWithTitle:@"OK"
                                                           style:UIAlertActionStyleDefault
                                                         handler:nil];
        [alert addAction:okAction];

        UIViewController *rootViewController = [UIApplication sharedApplication].keyWindow.rootViewController;

        while (rootViewController.presentedViewController) {
            rootViewController = rootViewController.presentedViewController;
        }

        [rootViewController presentViewController:alert animated:YES completion:nil];
    });
}

@end

static MediaPickerDelegate* mediaDelegate = nil;

void IOSBridge::importMediaFile(std::string type) {
    NSString* objectType = [NSString stringWithUTF8String:type.data()];

    if (!mediaDelegate) {
        mediaDelegate = [[MediaPickerDelegate alloc] init];
    }

    UIViewController *rootViewController = [UIApplication sharedApplication].keyWindow.rootViewController;

    if ([objectType isEqualToString:@"image"]) {
        // Изображения — через системную галерею (PHPicker), без запроса разрешений
        PHPickerConfiguration *config = [[PHPickerConfiguration alloc] init];
        config.filter = [PHPickerFilter imagesFilter];
        config.selectionLimit = 1;

        PHPickerViewController *picker = [[PHPickerViewController alloc] initWithConfiguration:config];
        picker.delegate = mediaDelegate;

        [rootViewController presentViewController:picker animated:YES completion:nil];
    }
    else {
        // Аудио и прочие типы — через Files
        NSArray<UTType *> *types;
        if ([objectType isEqualToString:@"audio"]) {
            types = @[UTTypeAudio];
        } else {
            types = @[UTTypeAudio, UTTypeImage];
        }

        UIDocumentPickerViewController *picker = [[UIDocumentPickerViewController alloc] initForOpeningContentTypes:types asCopy:NO];
        picker.delegate = mediaDelegate;

        [rootViewController presentViewController:picker animated:YES completion:nil];
    }
}

void IOSBridge::showNativeAlert(const std::string& title, const std::string& message) {
    NSString* nsTitle = [NSString stringWithUTF8String:title.c_str()];
    NSString* nsMessage = [NSString stringWithUTF8String:message.c_str()];

    [MediaPickerDelegate showAlertWithTitle:nsTitle message:nsMessage];
}