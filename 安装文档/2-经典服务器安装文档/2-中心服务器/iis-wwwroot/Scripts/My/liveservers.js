


//添加
function _AddLiveServer() {
    var _url = "AddLiveServer?name=" + encodeURI($('#name').val() + "&outip=" + $('#outip').val() + "&serial=" + $('#serial').val());
    //alert(_url);
    $.get(_url, function (response) {
        if ("0" != response) {
            alert("设备重复添加！请检查");
        }
    });
    $('#CreateServers').modal('toggle');
    $("#s_refresh").click();//刷新设备
}

//删除
var _coreserver_id;
function _DeleteLiveServer_Tmp(_id) {
    _coreserver_id = _id;
    $('#DeleteServers').modal('toggle');
}
function _DeleteLiveServer() {
    var _url = "DeleteLiveServer?id=" + _coreserver_id
    $.get(_url, function (response) {
    });
    $('#DeleteServers').modal('toggle');
    $("#s_refresh").click();//刷新设备
}


//=====================================================================
//设备管理
function _op_device(_id) {
    $("#svrid").val(_id);
    $("#s_refresh_device").click();//刷新设备
    $('#OpDevice').modal('toggle');
}

function _SelectUnusedDevice_Tmp() {
    $('#selecttree').tree('reload');
    $('#SelectDevice').modal('toggle')
}
function _SelectUnusedCheck() {
    var arr = [];
    var checkeds = $('#selecttree').tree('getChecked', 'checked');
    for (var i = 0; i < checkeds.length; i++) {
        arr.push(checkeds[i].id);
    }
    //alert(arr.join('-'));
    var didarr = arr.join('-');
    var _url = "LiveServerDeviceEdit?sid=" + $("#svrid").val() + "&didarr=" + didarr;
    //alert(_url);
    $.get(_url, function (response) {
        if ("0" != response) {
            alert(response);
        }
    });
    $('#SelectDevice').modal('toggle')
    $("#s_refresh_device").click();//刷新设备
}
function _DeleteDevice_Temp(_id) {
    var _url = "LiveServerDeviceDelete?did=" + _id;
    //alert(_url);
    $.get(_url, function (response) {
        if ("0" != response) {
            alert(response);
        }
    });

    $("#s_refresh_device").click();//刷新设备
}