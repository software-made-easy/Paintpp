QT       += core gui printsupport svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    Paint.h \
    about.h \
    dialog_windows.h \
    commands.h \
    draw_area.h \
    toolbar.h \
    tool.h \
    constants.h
SOURCES += main.cpp \
    Paint.cpp \
    about.cpp \
    commands.cpp \
    dialog_windows.cpp \
    toolbar.cpp \
    draw_area.cpp \
    tool.cpp

RESOURCES += \
    icons.qrc

TRANSLATIONS += \
    Paint_de.ts


CONFIG += lrelease
CONFIG += embed_translations

android: include(Qt-Color-Widgets/color_widgets.pri)
!unix || android {
    RC_ICONS = Paint.ico
    ICON = Paint.icns
}


VERSION = 0.1.0
DEFINES += APP_VERSION=\\\"$$VERSION\\\"
QMAKE_TARGET_COMPANY = "Software-made-easy"
QMAKE_TARGET_PRODUCT="Paint"
QMAKE_TARGET_DESCRIPTION="Paint is a simple tool for draw images."

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES +=

FORMS += \
    about.ui

DISTFILES += \
    android/AndroidManifest.xml \
    android/build.gradle \
    android/gradle.properties \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew \
    android/gradlew.bat \
    android/res/values/libs.xml

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
