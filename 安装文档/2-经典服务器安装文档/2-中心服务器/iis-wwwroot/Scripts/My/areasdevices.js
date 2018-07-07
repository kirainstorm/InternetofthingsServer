//
var _tree_loaded = 0;//树数据是否已经从后台读取完毕
var _selectednode;//保存选中的项

//===========================================================================================================
//选中了区域树
function _onSelectAreaTree(node) {
    _selectednode = node;//给selectednode赋值
    $('#area_name_edit').val(_selectednode.text);//给修改文本框赋值
    $("#searchareaid").val(_selectednode.id);//给设备搜索隐藏域赋值
    $("#s_refresh").click();//刷新设备
}
//区域树成功加载
function _onLoadSuccessAreaTree() {
    //这里为什么用了变量，因为append会触发onLoadSuccess
    if (0 == _tree_loaded) {
        $('#tt').tree('expandAll');//展开树
        _selectednode = $('#tt').tree('getRoot');////给selectednode赋值
        $('#tt').tree('select', _selectednode.target);//默认选中根目录
        _tree_loaded = 1;
    }
}
//在区域树上右键
function _onContextMenuAreaTree(e, node) {
    e.preventDefault();
    $('#tt').tree('select', node.target);//选中当前节点
    _selectednode = node;//给selectednode赋值
    //alert('--$$$' + node.id);
    $("#searchareaid").val(_selectednode.id);//给设备搜索隐藏域赋值
    $("#s_refresh").click();//刷新设备
    $('#area_name_edit').val(_selectednode.text);//给修改文本框赋值
    if (node == $('#tt').tree('getRoot')) {
        //根节点之允许修改/添加
        $('#mm1').menu('show', {
            left: e.pageX,
            top: e.pageY
        });
    } else if ($('#tt').tree('getParent', node.target) == $('#tt').tree('getRoot')) {
        $('#mm2').menu('show', {
            left: e.pageX,
            top: e.pageY
        });
    } else {
        //2级节点只允许修改/删除
        $('#mm3').menu('show', {
            left: e.pageX,
            top: e.pageY
        });
    }
}
//===========================================================================================================
function _AddArea() {
    var _url = "Home/AddArea?pid=" + encodeURI(_selectednode.id + "&name=" + $('#area_name_add').val());
    $.get(_url, function (response) {
        $('#tt').tree('append', {
            parent: (_selectednode ? _selectednode.target : null),
            data: [{
                id: response,//返回的response是插入数据的数据的ID 
                text: $('#area_name_add').val()
            }]
        });
        $('#area_name_add').val("");
    });
    $('#addArea').modal('toggle');
    $('#movetree').tree('reload');//重新加载移动树数据，此时其不可见取巧
}
function _EditArea() {
    var _url = "Home/EditArea?id=" + encodeURI(_selectednode.id + "&name=" + $('#area_name_edit').val());
    $.get(_url, function (response) {
        if ("0" == response) {
            _selectednode.text = $('#area_name_edit').val();
            $("#tt").tree("update", _selectednode);//更新
        }
        else {
            alert(response);
        }
        $('#editArea').modal('toggle');
        $('#movetree').tree('reload');//重新加载移动树数据，此时其不可见取巧
    });
}
function _DeleteArea() {
    var _url = "Home/DeleteArea?id=" + _selectednode.id;
    $.get(_url, function (response) {
        $('#deleteArea').modal('toggle');
        if ("0" == response) {
            var _removenode = _selectednode;
            $("#tt").tree("select", $("#tt").tree('getParent', _selectednode.target).target);//选中已删除的节点的父节点
            $('#tt').tree('remove', _removenode.target);//移除
        }
        else if ("-2" == response) {
            alert("有子项，不能删除！请先删除子项");
        }
        else {
            alert(response);
        }
    });
    $('#movetree').tree('reload');//重新加载移动树数据，此时其不可见取巧
}
//===========================================================================================================
var _device_id = 0;
function _AddDevice() {
    if ($('#device_name_add').val() == "") {
        alert("请填写名称");
        return;
    }
    if ($('#device_serial_add').val() == "") {
        alert("请填写序列号");
        return;
    }
    var _url = "Home/AddDevice?areaid=" + encodeURI($("#searchareaid").val() + "&disk=" + $('#device_disk_add').val() + "&serial=" + $('#device_serial_add').val() + "&name=" + $('#device_name_add').val() + "&mainsub=" + $('#device_mainsub_add').val());
    //alert(_url);
    $.get(_url, function (response) {
        // alert(response);
        if (response > 0) {

        }
        else {

        }
        $('#area_name_add').val("");//清空文本域
    });
    $('#addDevie').modal('toggle');
    $("#s_refresh").click();//刷新
}
function _EditDevice_Temp(_id, _disk, _name, __serial,_mainsub) {
    _device_id = _id;
    $('#device_disk_edit').val(_disk);
    $('#device_disk_edit').selectpicker('refresh');
    $('#device_name_edit').val(_name);
    $('#device_serial_edit').val(__serial);
    $('#device_mainsub_edit').val(_mainsub);
    $('#device_mainsub_edit').selectpicker('refresh');
    $('#EditDevice').modal('toggle');
}
function _EditDevice() {
    if ($('#device_name_edit').val() == "") {
        alert("请填写名称");
        return;
    }
    if ($('#device_serial_edit').val() == "") {
        alert("请填写序列号");
        return;
    }
    var _url = "Home/EditDevice?id=" + encodeURI(_device_id + "&disk=" + $('#device_disk_edit').val() + "&serial=" + $('#device_serial_edit').val() + "&name=" + $('#device_name_edit').val() + "&mainsub=" + $('#device_mainsub_edit').val());
    //alert(_url);
    $.get(_url, function (response) {
        //alert(response);
        if (response > 0) {

        }
        else {

        }
    });
    $('#EditDevice').modal('toggle');
    $("#s_refresh").click();//刷新
}
function _DeleteDevice_Temp(_id) {
    _device_id = _id;
    $('#DeleteDevice').modal('toggle');
}
function _DeleteDevice() {
    var _url = "Home/DeleteDevice?id=" + _device_id;
    $.get(_url, function (response) {
    });
    $('#DeleteDevice').modal('toggle');
    $("#s_refresh").click();//刷新
}
//===========================================================================================================
var _movedareaid;
function _MoveDevice_Temp(_id) {
    _device_id = _id;
    var _node = $('#movetree').tree('find', $("#searchareaid").val());//选中节点，不知道为什么这里用_selectednode.id不行
    $('#movetree').tree('select', _node.target);//选中节点
    $('#MoveDevice').modal('toggle');
}
function _MoveDevice() {
    $('#MoveDevice').modal('toggle');
}