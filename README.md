#Development pack for Zira PHP Editor.

This pack includes the following statically linked binaries:

* php 7.3.22 with built-in:
    * curl (with ftp, ftps, scp, sftp)
    * gd (with jpeg, png, freetype)
    * ftp
    * iconv
    * mbstring
    * mysqli
    * openssl
    * pdo (with mysqli, sqlite3)
    * sqlite3
    * xml
    * zip
    * web-server
* git 2.28.0
* sassc 3.6.1
* sqlite 3.33.0
* nano 5.2
* vim 8.2
* mc 4.8.25

##Download APK:
* [Android 5.0+ ARM-32bit](https://github.com/ziracms/devpack/releases/download/v0.0.2/ZiraDevPack-0.0.2-Android21-armv7.apk)
* [Android 5.0+ ARM-64bit](https://github.com/ziracms/devpack/releases/download/v0.0.2/ZiraDevPack-0.0.2-Android21-arm64-v8a.apk)
* [Android 5.0+ x86-32bit](https://github.com/ziracms/devpack/releases/download/v0.0.2/ZiraDevPack-0.0.2-Android21-x86.apk)
* [Android 5.0+ x86-64bit](https://github.com/ziracms/devpack/releases/download/v0.0.2/ZiraDevPack-0.0.2-Android21-x86-64.apk)
* [Android 4.1+ ARM-32bit](https://github.com/ziracms/devpack/releases/download/v0.0.2/ZiraDevPack-0.0.2-Android16-armv7.apk)
* [Android 4.1+ ARM-64bit](https://github.com/ziracms/devpack/releases/download/v0.0.2/ZiraDevPack-0.0.2-Android16-arm64-v8a.apk)

Choose the correct APK-file according to your system.

##Why this App is not on Google Play Store ?
This app targets API 28 and can't be uploaded to Google Play Store due to behavior changes in API 29.

[Read more >>](https://developer.android.com/about/versions/10/behavior-changes-10#execute-permission)

Though this pack is designed for Zira PHP Editor, it also can be used together with Android Terminal Emulator.

##How to edit php.ini with nano editor in Android Terminal Emulator:
* open Zira DevPack, copy environment initialization commands and paste them to Android Terminal Emulator
* don't forget to press Enter
* now type:

    > nano php.ini
    
* you can also use mc or vim

    > vim php.ini
    
##How to start PHP built-in web-server in Android Terminal Emulator:
* go to project directory, then type:

    > php -S 127.0.0.1:8000
    
* Now you can open your website in your favourite browser.
    
##How to upload project files to remote server:
* Midnight Commander (mc) has the built-in FTP and SFTP clients
    * type mc to open Midnight Commander 
    * then open menu (F9 -> FTP Link / SFTP Link).
    * enter remote server credentials and connect
    * now you can simply copy local files to remote server
