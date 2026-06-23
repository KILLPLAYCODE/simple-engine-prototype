/****************************************************************************
Copyright (c) 2015-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).
 
https://axmol.dev/

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
package dev.axmol.app;

import android.os.Bundle;
import dev.axmol.lib.AxmolActivity;
import dev.axmol.lib.SharedLoader;
import android.os.Build;
import android.view.WindowManager;
import android.view.WindowManager.LayoutParams;

public class AppActivity extends AxmolActivity {
    private static final int PICK_IMAGE = 1;
    private static AppActivity _instance = null;
    static {
        SharedLoader.load();
    }
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        _instance = this;
        if (!isTaskRoot()) {
            return;
        }
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
            WindowManager.LayoutParams lp = getWindow().getAttributes();
            lp.layoutInDisplayCutoutMode = WindowManager.LayoutParams.LAYOUT_IN_DISPLAY_CUTOUT_MODE_SHORT_EDGES;
            getWindow().setAttributes(lp);
        }
        
    }
    public static void openAndroidGallery(){
        if(_instance == null){
            return;
        }
        _instance.runOnUiThread(new Runnable(){
            @Override
            public void run(){
                Intent intent = new Intent(Intent.ACTION_GET_CONTENT);
                intent.setType("image/*");
                _instance.startActivityForResult(Intent.createChooser(intent, "Select Image", PICK_IMAGE));
            }
        });
    }
    @Override
    protected void OnActivityResult(int requestCode, int ResultCode, Intent data){
        super.OnActivityResult(requestCode,ResultCode,data);
        if(requestCode == PICK_IMAGE && ResultCode == Activity.RESULT_OK && data != null && data.getData() != null){
            Uri selectedImageUri = data.getData();
            InputStream inputStream = getContentResolver().openInputStream(selectedImageUri);
            File temp = new File(getCacheDir(), "icon.png");
            FileOutputStream outputStream = new FileOutputStream(temp);

            byte[] buffer = new byte[1024];
            int bytesRead;
            while((bytesRead = inputStream.read(buffer)) != -1){
                outputStream.write(buffer, 0, bytesRead);
            }

            inputStream.close();
            outputStream.close();

            onImageSelectedNative(temp.getAbsolutePath());
        }
    }
    private native void onImageSelectedNative(String path);
}
