
//
var _tree_loaded = 0;//树数据是否已经从后台读取完毕
var _selectednode;//保存选中的项

//===========================================================================================================
//选中了
function _onSelectAccountGroupTree(node) {
    _selectednode = node;//给selectednode赋值
    //alert(_selectednode.id);
    //alert(_selectednode.text);
    $('#group_name_edit').val(_selectednode.text);//给修改文本框赋值
    $("#searchaccountgroupidb").val(_selectednode.id);//给隐藏域赋值
    $("#s_refresh_b").click();//刷新
    $("#searchaccountgroupida").val(_selectednode.id);//给隐藏域赋值
    $("#s_refresh_a").click();//刷新
}
//成功加载
function _onLoadSuccessAccountGroupTree() {
    //这里为什么用了变量，因为append会触发onLoadSuccess
    if (0 == _tree_loaded) {
        $('#tt').tree('expandAll');//展开树
        _selectednode = $('#tt').tree('getRoot');////给selectednode赋值
        $('#tt').tree('select', _selectednode.target);//默认选中根目录
        _tree_loaded = 1;
    }
}
//右键
function _onContextMenuAccountGroupTree(e, node) {
    e.preventDefault();
    $('#tt').tree('select', node.target);//选中当前节点
    _selectednode = node;//给selectednode赋值
    //alert('--$$$' + node.id);

    $('#group_name_edit').val(_selectednode.text);//给修改文本框赋值
    $("#searchaccountgroupida").val(_selectednode.id);//给隐藏域赋值
    $("#s_refresh_a").click();//刷新
    $("#searchaccountgroupidb").val(_selectednode.id);//给隐藏域赋值
    $("#s_refresh_b").click();//刷新
    //alert(node.id);
    if (node.id == 1) {
        //根节点之允许修改/添加
        $('#mm1').menu('show', {
            left: e.pageX,
            top: e.pageY
        });
    } else {
        $('#mm2').menu('show', {
            left: e.pageX,
            top: e.pageY
        });
    }
}


//===========================================================================================================
//添加
function _AddAccountGroup() {
    var _url = "AccountGroups/AddAccountGroup?name=" + encodeURI($('#group_name_add').val());
    $.get(_url, function (response) {
        $('#tt').tree('append', {
            parent: (null),
            data: [{
                id: response,//返回的response是插入数据的数据的ID 
                text: $('#group_name_add').val()
            }]
        });
        $('#group_name_add').val("");
    });
    $('#addAccountGroup').modal('toggle');
    //$('#movetree').tree('reload');//重新加载移动树数据，此时其不可见取巧
}
//修改
function _EditAccountGroup() {
    var _url = "AccountGroups/EditAccountGroup?id=" + encodeURI(_selectednode.id + "&name=" + $('#group_name_edit').val());
    $.get(_url, function (response) {
        if ("0" == response) {
            _selectednode.text = $('#group_name_edit').val();
            $("#tt").tree("update", _selectednode);//更新
        }
        else {
            alert(response);
        }
        $('#editAccountGroup').modal('toggle');
        //$('#movetree').tree('reload');//重新加载移动树数据，此时其不可见取巧
    });
}
//删除
function _DeleteAccountGroup() {
    var _url = "AccountGroups/DeleteAccountGroup?id=" + _selectednode.id;
    $.get(_url, function (response) {
        $('#deleteAccountGroup').modal('toggle');
        if ("0" == response) {
            var _removenode = _selectednode;
            $("#tt").tree("select", $('#tt').tree('getRoot').target);
            $('#tt').tree('remove', _removenode.target);//移除
        }
        else {
            alert(response);
        }
    });
    //$('#movetree').tree('reload');//重新加载移动树数据，此时其不可见取巧
}
//移动
/*
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
*/

//===================================================================================================================================================
var _accout_id = 0;
//添加
function _AddAccount() {
    var _url = "AccountGroups/AddAccount?gid=" + encodeURI(_selectednode.id + "&name=" + $('#account_name_add').val() + "&phone=" + $('#account_phone_add').val());
    //alert(_url);
    $.get(_url, function (response) {
        if ("0" != response) {
            alert(response);
        }
    });
    $('#addAccount').modal('toggle');
    $("#s_refresh_a").click();//刷新
}
//修改
function _EditAccount_Temp(_id, _name, _phone) {
    _accout_id = _id;
    $('#account_name_edit').val(_name);
    $('#account_phone_edit').val(_phone);
    $('#editAccount').modal('toggle');
}
function _EditAccount() {
    var _url = "AccountGroups/EditAccount?id=" + encodeURI(_accout_id + "&name=" + $('#account_name_edit').val() + "&phone=" + $('#account_phone_edit').val());
    $.get(_url, function (response) {
        if ("0" != response) {
            alert(response);
        }
    });
    $('#editAccount').modal('toggle');
    $("#s_refresh_a").click();//刷新
}
//删除
function _DeleteAccount_Temp(_id) {
    _accout_id = _id;
    $('#deleteAccount').modal('toggle');
}
function _DeleteAccount() {
    var _url = "AccountGroups/DeleteAccount?id=" + _accout_id;
    $.get(_url, function (response) {
        if ("0" != response)
        {
            alert(response);
        }
    });
    $('#deleteAccount').modal('toggle');
    $("#s_refresh_a").click();//刷新
}
//=============================================================================
var _checked_device_node = [3,5];
var _accountgroupdevice_id;
function _Travel_Checked() {
    var roots = $('#selecttree').tree('getRoots'), children, i, j;
    for (i = 0; i < roots.length; i++) {
        //alert(roots[i].text);

        var bfind = false;
        for(n = 0; n < _checked_device_node.length ; n++)
        {
            if(roots[i].id == _checked_device_node[n])
            {
                bfind = true;
                break;
            }
        }

        if (!bfind) {
            $('#selecttree').tree('uncheck', roots[i].target);
        }
        else {
            $('#selecttree').tree('check', roots[i].target);
        }
    }
}


function _SelectNodeCheck_Tmp()
{
    _Travel_Checked();
    $('#SelectDevice').modal('toggle')

}


function _SelectNodeCheck()
{

    var arr = [];
    
    var checkeds = $('#selecttree').tree('getChecked', 'checked');
    for (var i = 0; i < checkeds.length; i++) {
        arr.push(checkeds[i].id);
    }
    
    //alert(arr.join('-'));
    
    var didarr = arr.join('-');

    var _url = "AccountGroups/AccountGroupDeviceEdit?gid=" + _selectednode.id + "&didarr=" + didarr;
    //alert(_url);
    $.get(_url, function (response) {
        if ("0" != response) {
            alert(response);
        }
    });
    $('#SelectDevice').modal('toggle');
    $("#s_refresh_b").click();//刷新
}
//删除
function _DeleteAccountGroupDevice_Temp(_id) {
    _accountgroupdevice_id = _id;
    $('#deleteAccountGroupDevice').modal('toggle');
}
function _DeleteAccountGroupDevice() {
    var _url = "AccountGroups/DeleteAccountGroupDevice?gid=" + _selectednode.id + "&did=" + _accountgroupdevice_id;
    $.get(_url, function (response) {
        if ("0" != response) {
            alert(response);
        }
    });
    $('#deleteAccountGroupDevice').modal('toggle');
    $("#s_refresh_b").click();//刷新
}
