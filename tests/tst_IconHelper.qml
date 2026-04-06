import QtQuick
import QtTest
import "../package/contents/ui/lib/IconHelper.js" as IconHelper

Item {
    TestCase {
        name: "IconHelperTests"

        function test_directoryIcon() {
            compare(IconHelper.iconForFile("Documents", true), "folder");
        }

        function test_desktopFileIcon() {
            compare(IconHelper.iconForFile("firefox.desktop", false), "application-x-desktop");
        }

        function test_textFileIcon() {
            compare(IconHelper.iconForFile("readme.txt", false), "text-plain");
        }

        function test_pdfFileIcon() {
            compare(IconHelper.iconForFile("document.pdf", false), "application-pdf");
        }

        function test_pngFileIcon() {
            compare(IconHelper.iconForFile("photo.png", false), "image-png");
        }

        function test_jpegFileIcon() {
            compare(IconHelper.iconForFile("photo.jpg", false), "image-jpeg");
        }

        function test_archiveIcon() {
            compare(IconHelper.iconForFile("data.zip", false), "application-zip");
        }

        function test_shellScriptIcon() {
            compare(IconHelper.iconForFile("run.sh", false), "application-x-shellscript");
        }

        function test_unknownExtension() {
            compare(IconHelper.iconForFile("data.xyz123", false), "application-octet-stream");
        }

        function test_caseInsensitive() {
            compare(IconHelper.iconForFile("README.TXT", false), "text-plain");
            compare(IconHelper.iconForFile("Photo.PNG", false), "image-png");
        }

        function test_noExtension() {
            compare(IconHelper.iconForFile("Makefile", false), "application-octet-stream");
        }

        function test_parseDesktopEntry() {
            var content = "[Desktop Entry]\nName=Firefox\nIcon=firefox\nExec=firefox %u\nType=Application";
            var result = IconHelper.parseDesktopEntry(content);
            compare(result.name, "Firefox");
            compare(result.icon, "firefox");
            compare(result.exec, "firefox %u");
        }

        function test_parseDesktopEntryEmpty() {
            var result = IconHelper.parseDesktopEntry("");
            compare(result.name, "");
            compare(result.icon, "application-x-desktop");
        }

        function test_parseDesktopEntryOnlyName() {
            var content = "[Desktop Entry]\nName=MyApp\nType=Application";
            var result = IconHelper.parseDesktopEntry(content);
            compare(result.name, "MyApp");
            compare(result.icon, "application-x-desktop");
        }

        function test_parseDesktopEntryUsesFirstName() {
            var content = "[Desktop Entry]\nName=English\nName[de]=Deutsch\nIcon=app";
            var result = IconHelper.parseDesktopEntry(content);
            compare(result.name, "English");
        }

        function test_parseDesktopEntryIgnoresActionSections() {
            var content = "[Desktop Entry]\nName=Firefox\nIcon=firefox\nExec=firefox\n\n[Desktop Action NewWindow]\nName=New Window\nIcon=window-new\nExec=firefox --new-window";
            var result = IconHelper.parseDesktopEntry(content);
            compare(result.name, "Firefox");
            compare(result.icon, "firefox");
            compare(result.exec, "firefox");
        }
    }
}
