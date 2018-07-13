using NewPlatCenter.Models;
using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.Linq;
using System.Web;

namespace NewPlatCenter.ViewModels
{
    //要想用easyui-tree，，必须使用固定格式id，text，parent，children
    public class LiveDeviceModels
    {
        public List<Device> a { get; set; }
        public List<Device> b { get; set; }
    }
}