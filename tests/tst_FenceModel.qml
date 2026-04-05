import QtQuick
import QtTest
import "../package/contents/ui/lib/FenceModel.js" as FenceModel

Item {
    TestCase {
        name: "FenceModelTests"

        function test_loadEmptyString() {
            var fences = FenceModel.loadFences("");
            compare(fences.length, 0);
        }

        function test_loadNull() {
            var fences = FenceModel.loadFences(null);
            compare(fences.length, 0);
        }

        function test_loadUndefined() {
            var fences = FenceModel.loadFences(undefined);
            compare(fences.length, 0);
        }

        function test_loadInvalidJson() {
            var fences = FenceModel.loadFences("{bad json!!!");
            compare(fences.length, 0);
        }

        function test_createFence() {
            var fences = [];
            var fence = FenceModel.createFence(fences, 100, 200, 400, 300, "Test Fence");
            compare(fences.length, 1);
            compare(fence.title, "Test Fence");
            compare(fence.x, 100);
            compare(fence.y, 200);
            compare(fence.width, 400);
            compare(fence.height, 300);
            compare(fence.rolledUp, false);
            compare(fence.iconSize, 64);
            compare(fence.icons.length, 0);
            verify(fence.id.length > 0);
        }

        function test_createFenceDefaultTitle() {
            var fences = [];
            var fence = FenceModel.createFence(fences, 0, 0, 400, 300, "");
            compare(fence.title, "New Fence");
        }

        function test_createFenceEnforcesMinSize() {
            var fences = [];
            var fence = FenceModel.createFence(fences, 0, 0, 50, 30, "Tiny");
            verify(fence.width >= 200);
            verify(fence.height >= 150);
        }

        function test_deleteFence() {
            var fences = [];
            var fence = FenceModel.createFence(fences, 0, 0, 400, 300, "ToDelete");
            FenceModel.assignIcon(fences, fence.id, "file:///test.txt", 0, 0);
            var orphans = FenceModel.deleteFence(fences, fence.id);
            compare(fences.length, 0);
            compare(orphans.length, 1);
            compare(orphans[0].url, "file:///test.txt");
        }

        function test_deleteFenceNotFound() {
            var fences = [];
            var orphans = FenceModel.deleteFence(fences, "nonexistent");
            compare(orphans.length, 0);
        }

        function test_findFence() {
            var fences = [];
            var f1 = FenceModel.createFence(fences, 0, 0, 400, 300, "F1");
            var f2 = FenceModel.createFence(fences, 0, 0, 400, 300, "F2");
            var found = FenceModel.findFence(fences, f1.id);
            compare(found.title, "F1");
            var notFound = FenceModel.findFence(fences, "bogus");
            compare(notFound, null);
        }

        function test_assignIcon() {
            var fences = [];
            var fence = FenceModel.createFence(fences, 0, 0, 400, 300, "F");
            var ok = FenceModel.assignIcon(fences, fence.id, "file:///a.txt", 0, 0);
            verify(ok);
            compare(fence.icons.length, 1);
            compare(fence.icons[0].url, "file:///a.txt");
            compare(fence.icons[0].col, 0);
            compare(fence.icons[0].row, 0);
        }

        function test_assignIconToNonexistentFence() {
            var fences = [];
            var ok = FenceModel.assignIcon(fences, "bogus", "file:///a.txt", 0, 0);
            verify(!ok);
        }

        function test_assignIconMovesFromOtherFence() {
            var fences = [];
            var f1 = FenceModel.createFence(fences, 0, 0, 400, 300, "F1");
            var f2 = FenceModel.createFence(fences, 0, 0, 400, 300, "F2");
            FenceModel.assignIcon(fences, f1.id, "file:///a.txt", 0, 0);
            compare(f1.icons.length, 1);
            FenceModel.assignIcon(fences, f2.id, "file:///a.txt", 1, 1);
            compare(f1.icons.length, 0);
            compare(f2.icons.length, 1);
            compare(f2.icons[0].col, 1);
        }

        function test_unassignIconFromAll() {
            var fences = [];
            var f1 = FenceModel.createFence(fences, 0, 0, 400, 300, "F1");
            FenceModel.assignIcon(fences, f1.id, "file:///a.txt", 0, 0);
            FenceModel.assignIcon(fences, f1.id, "file:///b.txt", 1, 0);
            FenceModel.unassignIconFromAll(fences, "file:///a.txt");
            compare(f1.icons.length, 1);
            compare(f1.icons[0].url, "file:///b.txt");
        }

        function test_nextEmptyCell() {
            var fences = [];
            var fence = FenceModel.createFence(fences, 0, 0, 400, 300, "F");
            var cell = FenceModel.nextEmptyCell(fence);
            compare(cell.col, 0);
            compare(cell.row, 0);

            FenceModel.assignIcon(fences, fence.id, "file:///a.txt", 0, 0);
            cell = FenceModel.nextEmptyCell(fence);
            compare(cell.col, 1);
            compare(cell.row, 0);
        }

        function test_nextEmptyCellWrapsToNextRow() {
            var fences = [];
            var fence = FenceModel.createFence(fences, 0, 0, 200, 300, "F");
            FenceModel.assignIcon(fences, fence.id, "file:///a.txt", 0, 0);
            FenceModel.assignIcon(fences, fence.id, "file:///b.txt", 1, 0);
            var cell = FenceModel.nextEmptyCell(fence);
            compare(cell.col, 0);
            compare(cell.row, 1);
        }

        function test_allAssignedUrls() {
            var fences = [];
            var f1 = FenceModel.createFence(fences, 0, 0, 400, 300, "F1");
            var f2 = FenceModel.createFence(fences, 0, 0, 400, 300, "F2");
            FenceModel.assignIcon(fences, f1.id, "file:///a.txt", 0, 0);
            FenceModel.assignIcon(fences, f2.id, "file:///b.txt", 0, 0);
            var urls = FenceModel.allAssignedUrls(fences);
            verify(urls["file:///a.txt"]);
            verify(urls["file:///b.txt"]);
            verify(!urls["file:///c.txt"]);
        }

        function test_removeDeletedFiles() {
            var fences = [];
            var fence = FenceModel.createFence(fences, 0, 0, 400, 300, "F");
            FenceModel.assignIcon(fences, fence.id, "file:///exists.txt", 0, 0);
            FenceModel.assignIcon(fences, fence.id, "file:///gone.txt", 1, 0);
            FenceModel.removeDeletedFiles(fences, ["file:///exists.txt"]);
            compare(fence.icons.length, 1);
            compare(fence.icons[0].url, "file:///exists.txt");
        }

        function test_roundTripSerialization() {
            var fences = [];
            FenceModel.createFence(fences, 100, 200, 400, 300, "Serialized");
            FenceModel.assignIcon(fences, fences[0].id, "file:///b.txt", 2, 1);
            var json = FenceModel.saveFences(fences);
            var loaded = FenceModel.loadFences(json);
            compare(loaded.length, 1);
            compare(loaded[0].title, "Serialized");
            compare(loaded[0].x, 100);
            compare(loaded[0].y, 200);
            compare(loaded[0].icons.length, 1);
            compare(loaded[0].icons[0].col, 2);
            compare(loaded[0].icons[0].row, 1);
        }
    }
}
