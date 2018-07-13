using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;



namespace NewPlatCenter.Models
{
    public class Account
    {

        //由管理员添加的用户（比如家长）
        [Key]
        public int ID { get; set; }

        [Required]
        public int AcountGroupID { get; set; }//用户组ID

        [Required]
        [StringLength(32)]
        public string Ac_Phone { get; set; }//account name  手机号码

        [Required]
        [StringLength(128)]
        public string Ac_Pwd { get; set; }//account password 默认

        [Required]
        [StringLength(64)]
        public string Ac_Nickname { get; set; }
    }
}