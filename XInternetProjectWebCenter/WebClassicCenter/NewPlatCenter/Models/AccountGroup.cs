using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;

namespace NewPlatCenter.Models
{
    public class AccountGroup
    {
        [Key]
        public int ID { get; set; }
        [Required]
        [StringLength(64)]
        public string Ag_Name { get; set; }//组名称
    }
}