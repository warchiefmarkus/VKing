QT      +=  core network xml gui
HEADERS =   DataForm.h \
    json.h \
    k8json/k8json.h \
    mainwindow.h \
    downloadmanager.h
SOURCES =   main.cpp \
            DataForm.cpp \
    json.cpp \
    k8json/k8json.cpp \
    mainwindow.cpp \
    downloadmanager.cpp


# install
INSTALLS += target sources
VERSION = 1.5

win32{
TARGET = VKing_$${VERSION}
}

symbian {
TARGET = VKing
!symbian: error(Only include this file for Symbian platforms)
RSS_RULES ="group_name=\"VK\";"
nokiaVendor = "Nokia, Qt"
vendorinfo = \
    "; Localised Vendor name" \
    "%{$$addLanguageDependentPkgItem(nokiaVendor)}" \
    " " \
    "; Unique Vendor name" \
    ":\"$$nokiaVendor\"" \
    " "
examples_deployment.pkg_prerules += vendorinfo
DEPLOYMENT += examples_deployment

isEmpty(ICON):contains(TEMPLATE, ".*app"):contains(QT, gui):contains(CONFIG, qt):!contains(CONFIG, "no_icon") {
     ICON = $$PWD/vking.svg
}
    TARGET.UID3 = 0xA000CF6C
    TARGET.CAPABILITY = NetworkServices
    #S60_VERSION = 3.0
    #SYMBIAN_VERSION = 9.2
    TARGET.EPOCSTACKSIZE = 0xFFFF
    TARGET.EPOCHEAPSIZE = 0x020000 0x5000000
}

