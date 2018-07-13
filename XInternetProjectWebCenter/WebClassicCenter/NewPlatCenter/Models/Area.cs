using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;

namespace NewPlatCenter.Models
{
    public class Area
    {
        [Key]
        //--------------------------------------id
        public int ID { get; set; }
        [Required]
        //--------------------------------------name
        [StringLength(64)]
        public string Ar_Name { get; set; }
        [Required]
        //--------------------------------------parent id
        public int Ar_ParentID { get; set; }
        //--------------------------------------is root?
        [Required]
        public int Ar_Admin { get; set; } //1:创建Administrator的时候创建的，不可以删除， 0:用户手动创建的，可以自由删除 //等于公司名称
    }
}