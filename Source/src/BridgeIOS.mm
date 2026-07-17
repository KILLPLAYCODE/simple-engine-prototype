#import "IOSBridge.h"
#import <UIKit/UIKit.h>
#import <UniformTypeIdentifiers/UniformTypeIdentifiers.h>

// Класс-делегат, который поймает событие выбора файла в iOS
@interface DocumentPickerDelegate : NSObject <UIDocumentPickerDelegate>
@end

@implementation DocumentPickerDelegate

- (void)documentPicker:(UIDocumentPickerViewController *)controller didPickDocumentsAtURLs:(NSArray<NSURL *> *)urls {
    NSURL *selectedURL = [urls firstObject];
    if (!selectedURL) return;

    // iOS требует запросить безопасный доступ к файлу из чужого пространства
    if ([selectedURL startAccessingSecurityScopedResource]) {
        
        // Получаем путь к папке кэша или документов вашей игры в Axmol
        NSArray *paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
        NSString *cacheDirectory = [paths objectAtIndex:0];
        NSString *fileName = [selectedURL lastPathComponent];
        NSString *destinationPath = [cacheDirectory stringByAppendingPathComponent:fileName];
        
        // Копируем файл в папку игры, так как напрямую из iCloud/Медиатеки Axmol его не прочитает
        NSFileManager *fileManager = [NSFileManager defaultManager];
        NSError *error = nil;
        
        if ([fileManager fileExistsAtPath:destinationPath]) {
            [fileManager removeItemAtPath:destinationPath error:nil];
        }
        
        [fileManager copyItemAtPath:[selectedURL path] toPath:destinationPath error:&error];
        [selectedURL stopAccessingSecurityScopedResource];
        
        if (!error) {
            // Передаем путь обратно в ваш C++ код Axmol
            std::string cppPath = [destinationPath UTF8String];
            IOSBridge::onFileSelected(cppPath);
        }
    }
}
@end

// Статическое хранилище для делегата, чтобы он не удалился из памяти
static DocumentPickerDelegate* pickerDelegate = nil;

void IOSBridge::importMediaFile() {
    // Указываем, какие типы файлов нам нужны (например, изображения и аудио)
    NSArray<UTType *> *types = @[UTTypeImage, UTTypeAudio];
    
    UIDocumentPickerViewController *picker = [[UIDocumentPickerViewController alloc] initWithContentTypes:types asCopy:YES];
    
    if (!pickerDelegate) {
        pickerDelegate = [[DocumentPickerDelegate alloc] init];
    }
    picker.delegate = pickerDelegate;
    
    // Получаем главное окно игры Axmol для вывода системного экрана поверх неё
    UIViewController *rootViewController = [UIApplication sharedApplication].keyWindow.rootViewController;
    [rootViewController presentViewController:picker animated:YES completion:nil];
}
