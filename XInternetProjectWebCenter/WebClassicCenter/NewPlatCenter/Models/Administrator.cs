using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;

namespace NewPlatCenter.Models
{
    public class Administrator
    {
        //管理员账户，只允许使用该账户登陆后台
        //暂时有且只有一个

        [Key]
        public int ID { get; set; }
        [Required]
        [StringLength(32)]
        public string Admin_Name { get; set; }
        [Required]
        [StringLength(128)]
        public string Admin_Pwd { get; set; }
    }
}