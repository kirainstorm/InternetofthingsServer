using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.Linq;
using System.Web;

namespace NewPlatCenter.ViewModels
{
    //要想用easyui-tree，，必须使用固定格式id，text，parent，children
    public class AccountGroupViewModels
    {
        [Key]
        public int id { get; set; }
        public string text { get; set; }
        public int parent { get; set; }

        public List<AccountGroupViewModels> children { get; set; }
    }
}