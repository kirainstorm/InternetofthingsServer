using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;

namespace NewPlatCenter.Models
{
    public class AccountGroupDevice
    {
        //用户组关联的设备

        [Key]
        public int ID { get; set; }

        [Required]
        public int AcountGroupID { get; set; }//用户组ID

        [Required]
        public int DeviceID { get; set; }//设备ID
    }
}