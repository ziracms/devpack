QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

equals(QT_MAJOR_VERSION, 5): lessThan(QT_MINOR_VERSION, 13) {
    DISTFILES += \
        android/Qt5_12/AndroidManifest.xml \
        android/Qt5_12/build.gradle \
        android/Qt5_12/gradle/wrapper/gradle-wrapper.jar \
        android/Qt5_12/gradle/wrapper/gradle-wrapper.properties \
        android/Qt5_12/gradlew \
        android/Qt5_12/gradlew.bat \
        android/Qt5_12/res/values/libs.xml

    contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
        ANDROID_PACKAGE_SOURCE_DIR = \
            $$PWD/android/Qt5_12
    }

    contains(ANDROID_TARGET_ARCH,arm64-v8a) {
        ANDROID_PACKAGE_SOURCE_DIR = \
            $$PWD/android/Qt5_12
    }
}

equals(QT_MAJOR_VERSION, 5): greaterThan(QT_MINOR_VERSION, 12) {
    DISTFILES += \
        android/Qt5_14/AndroidManifest.xml \
        android/Qt5_14/build.gradle \
        android/Qt5_14/gradle/wrapper/gradle-wrapper.jar \
        android/Qt5_14/gradle/wrapper/gradle-wrapper.properties \
        android/Qt5_14/gradlew \
        android/Qt5_14/gradlew.bat \
        android/Qt5_14/res/values/libs.xml

    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android/Qt5_14
}

RESOURCES += \
    qrc/images.qrc \
    qrc/styles.qrc
