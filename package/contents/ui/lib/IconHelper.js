.pragma library

var _extensionIconMap = {
    // Documents
    "txt": "text-plain", "pdf": "application-pdf",
    "doc": "application-msword", "docx": "application-msword",
    "odt": "application-vnd.oasis.opendocument.text",
    "xls": "application-vnd.ms-excel", "xlsx": "application-vnd.ms-excel",
    "ods": "application-vnd.oasis.opendocument.spreadsheet",
    "ppt": "application-vnd.ms-powerpoint", "pptx": "application-vnd.ms-powerpoint",
    "odp": "application-vnd.oasis.opendocument.presentation",
    "csv": "text-csv",
    // Images
    "png": "image-png", "jpg": "image-jpeg", "jpeg": "image-jpeg",
    "gif": "image-gif", "svg": "image-svg+xml", "webp": "image-webp",
    "bmp": "image-bmp", "ico": "image-x-ico",
    // Audio
    "mp3": "audio-mpeg", "flac": "audio-flac", "ogg": "audio-ogg",
    "wav": "audio-x-wav", "m4a": "audio-mp4", "opus": "audio-opus",
    // Video
    "mp4": "video-mp4", "mkv": "video-x-matroska",
    "avi": "video-x-msvideo", "webm": "video-webm", "mov": "video-quicktime",
    // Archives
    "zip": "application-zip", "tar": "application-x-tar",
    "gz": "application-gzip", "xz": "application-x-xz",
    "bz2": "application-x-bzip2", "7z": "application-x-7z-compressed",
    "rar": "application-x-rar", "zst": "application-zstd",
    // Code
    "py": "text-x-python", "js": "text-javascript", "ts": "text-x-typescript",
    "html": "text-html", "htm": "text-html",
    "css": "text-css", "json": "application-json", "xml": "text-xml",
    "sh": "application-x-shellscript", "bash": "application-x-shellscript",
    "cpp": "text-x-c++src", "c": "text-x-csrc",
    "h": "text-x-chdr", "hpp": "text-x-c++hdr",
    "rs": "text-rust", "go": "text-x-go",
    "java": "text-x-java", "kt": "text-x-kotlin",
    "rb": "text-x-ruby", "php": "text-x-php",
    "md": "text-markdown", "rst": "text-x-rst",
    "yaml": "text-yaml", "yml": "text-yaml", "toml": "application-toml",
    // Packages
    "deb": "application-vnd.debian.binary-package",
    "rpm": "application-x-rpm",
    "appimage": "application-x-executable",
    "flatpakref": "application-vnd.flatpak.ref",
    // Disk images
    "iso": "application-x-cd-image",
    // Desktop entries
    "desktop": "application-x-desktop"
};

function iconForFile(fileName, isDir) {
    if (isDir) return "folder";

    var dotIdx = fileName.lastIndexOf('.');
    if (dotIdx === -1 || dotIdx === fileName.length - 1) return "application-octet-stream";

    var ext = fileName.substring(dotIdx + 1).toLowerCase();
    return _extensionIconMap[ext] || "application-octet-stream";
}

function parseDesktopEntry(content) {
    var result = { name: "", icon: "application-x-desktop", exec: "" };
    if (!content) return result;

    var lines = content.split('\n');
    for (var i = 0; i < lines.length; i++) {
        var line = lines[i].trim();
        if (line.indexOf("Name=") === 0 && !result.name) {
            result.name = line.substring(5);
        } else if (line.indexOf("Icon=") === 0) {
            result.icon = line.substring(5);
        } else if (line.indexOf("Exec=") === 0 && !result.exec) {
            result.exec = line.substring(5);
        }
    }
    return result;
}

function loadDesktopFileInfo(fileUrl, callback) {
    var xhr = new XMLHttpRequest();
    xhr.onreadystatechange = function() {
        if (xhr.readyState === XMLHttpRequest.DONE) {
            callback(parseDesktopEntry(xhr.responseText));
        }
    };
    xhr.open("GET", fileUrl);
    xhr.send();
}
