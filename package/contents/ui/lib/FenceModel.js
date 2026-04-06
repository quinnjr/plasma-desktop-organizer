.pragma library

function generateId() {
    return 'xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx'.replace(/[xy]/g, function(c) {
        var r = Math.random() * 16 | 0;
        var v = c === 'x' ? r : (r & 0x3 | 0x8);
        return v.toString(16);
    });
}

function loadFences(configString) {
    if (!configString) return [];
    try {
        return JSON.parse(configString);
    } catch (e) {
        console.warn("FenceModel: failed to parse fences config:", e);
        return [];
    }
}

function saveFences(fences) {
    return JSON.stringify(fences);
}

function createFence(fences, x, y, width, height, title) {
    var fence = {
        id: generateId(),
        title: (title && title.length > 0) ? title : "New Fence",
        x: x,
        y: y,
        width: Math.max(width, 200),
        height: Math.max(height, 150),
        rolledUp: false,
        iconSize: 64,
        icons: []
    };
    fences.push(fence);
    return fence;
}

function deleteFence(fences, fenceId) {
    var idx = indexOfFence(fences, fenceId);
    if (idx === -1) return [];
    var icons = fences[idx].icons.slice();
    fences.splice(idx, 1);
    return icons;
}

function findFence(fences, fenceId) {
    for (var i = 0; i < fences.length; i++) {
        if (fences[i].id === fenceId) return fences[i];
    }
    return null;
}

function indexOfFence(fences, fenceId) {
    for (var i = 0; i < fences.length; i++) {
        if (fences[i].id === fenceId) return i;
    }
    return -1;
}

function assignIcon(fences, fenceId, url, col, row) {
    var fence = findFence(fences, fenceId);
    if (!fence) return false;
    unassignIconFromAll(fences, url);
    fence.icons.push({ url: url, col: col, row: row });
    return true;
}

function unassignIconFromAll(fences, url) {
    for (var i = 0; i < fences.length; i++) {
        fences[i].icons = fences[i].icons.filter(function(icon) {
            return icon.url !== url;
        });
    }
}

function nextEmptyCell(fence) {
    var spacing = 16;
    var iconMargin = 16;
    var cols = Math.max(1, Math.floor(fence.width / (fence.iconSize + spacing + iconMargin)));
    var occupied = {};
    for (var i = 0; i < fence.icons.length; i++) {
        var icon = fence.icons[i];
        occupied[icon.col + "," + icon.row] = true;
    }
    for (var row = 0; row < 1000; row++) {
        for (var col = 0; col < cols; col++) {
            if (!occupied[col + "," + row]) {
                return { col: col, row: row };
            }
        }
    }
    return { col: 0, row: 0 };
}

function allAssignedUrls(fences) {
    var urls = {};
    for (var i = 0; i < fences.length; i++) {
        for (var j = 0; j < fences[i].icons.length; j++) {
            urls[fences[i].icons[j].url] = true;
        }
    }
    return urls;
}

function removeDeletedFiles(fences, currentUrls) {
    var urlSet = {};
    for (var i = 0; i < currentUrls.length; i++) {
        urlSet[currentUrls[i]] = true;
    }
    for (var j = 0; j < fences.length; j++) {
        fences[j].icons = fences[j].icons.filter(function(icon) {
            return urlSet[icon.url] === true;
        });
    }
}
