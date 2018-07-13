using System;
using System.Collections.Generic;
using System.Data;
using System.Data.Entity;
using System.Linq;
using System.Net;
using System.Web;
using System.Web.Mvc;
using NewPlatCenter.Models;
using NewPlatCenter.ViewModels;

namespace NewPlatCenter.Controllers
{
    public class AccountGroupsController : Controller
    {
        private PlatCenterEntities db = new PlatCenterEntities();

        // GET: AccountGroups
        public ActionResult Index()
        {

            if (Session["userID"] == null)
                return RedirectToAction("Index", "Login");
            return View();
        }



        public JsonResult AccountGroupsJsonTree()
        {

            // if (Session["userName"] == null)
            //     return Json("error", JsonRequestBehavior.AllowGet);

            List<AccountGroup> items = null;
            items = db.AccountGroups.OrderBy(ss => ss.ID).ToList();

            //
            List<AccountGroupViewModels> hierarchicalItems = items
                .ToList().Select(c => new AccountGroupViewModels
                {
                    id = c.ID,
                    text = c.Ag_Name,
                    parent = 0
                }).ToList();
            //
            var s = hierarchicalItems.Select(a => new
            {
                a.id,
                a.text,
                a.children
            });

            JsonResult jsr = Json(s, JsonRequestBehavior.AllowGet);
            return jsr;
        }




        //=======================================================================================================================================
        [HttpGet]
        public string AddAccountGroup(string name)
        {
            if (name.Length <= 0)
            {
                return "-1";
            }
            AccountGroup _aag = new AccountGroup()
            {
                Ag_Name = HttpUtility.HtmlDecode(name),
            };
            db.AccountGroups.Add(_aag);
            db.SaveChanges();//EF插入的时候会自动返回并赋值给原来的实体对象。也就是这个对象savechange后，自增ID就有值了
            return _aag.ID.ToString();
        }


        [HttpGet]
        public string EditAccountGroup(int id, string name)
        {
            List<AccountGroup> _ag = (from m in db.AccountGroups
                                      where ((m.ID == id))
                                      select m).ToList();
            if (1 != _ag.Count())
            {
                return "-1";
            }

            if (name.Length <= 0)
            {
                return "-2";
            }

            _ag[0].Ag_Name = HttpUtility.HtmlDecode(name);
            db.Entry(_ag[0]).State = EntityState.Modified;
            db.SaveChanges();

            return "0";
        }

        [HttpGet]
        public string DeleteAccountGroup(int id)
        {
            if (1 == id)
            {
                //不允许删除
                return "-1";
            }


            //下面有设备
            List<AccountGroupDevice> _d = (from m in db.AccountGroupDevices
                                           where (m.AcountGroupID == id)
                                           select m).ToList();
            if (_d.Count() > 0)
            {
                for (int n = 0; n < _d.Count; n++)
                {
                    db.Entry(_d[n]).State = EntityState.Deleted;
                }
                db.SaveChanges();
            }

            db.AccountGroups.Remove(db.AccountGroups.Find(id));
            db.SaveChanges();

            return "0";
        }


        //=======================================================================================================================================




        public PartialViewResult _PartialPageAccountList(int? searchaccountgroupida)
        {
            int _id = 0;
            if (null == searchaccountgroupida)
            {
                _id = 1;//取巧//取默认区域数据//一定返回某个值
            }
            else
            {
                _id = searchaccountgroupida.Value;
            }

            //ViewBag.areaid = searchareaid;
            var _sev = from s in db.Accounts where (s.AcountGroupID == _id) orderby s.ID select s;
            return PartialView(_sev.ToList());
        }

        [HttpGet]
        public string AddAccount(int gid ,string name, string phone)
        {
//             if(null == gid)
//             {
//                 return "-1";
//             }
            if (name.Length <= 0)
            {
                return "-2";
            }
            if (phone.Length <= 0)
            {
                return "-3";
            }
            Account _aag = new Account()
            {
                AcountGroupID = gid,
                Ac_Nickname = HttpUtility.HtmlDecode(name),
                Ac_Phone = HttpUtility.HtmlDecode(phone),
                Ac_Pwd = HttpUtility.HtmlDecode(phone)
            };
            db.Accounts.Add(_aag);
            db.SaveChanges();//EF插入的时候会自动返回并赋值给原来的实体对象。也就是这个对象savechange后，自增ID就有值了
            return "0";// _aag.ID.ToString();
        }


        [HttpGet]
        public string EditAccount(int id, string name, string phone)
        {
//             if (null == id)
//             {
//                 return "-1";
//             }
            if (name.Length <= 0)
            {
                return "-2";
            }
            if (phone.Length <= 0)
            {
                return "-3";
            }


            List<Account> _ag = (from m in db.Accounts
                                      where ((m.ID == id))
                                      select m).ToList();
            if (1 != _ag.Count())
            {
                return "-4";
            }

            _ag[0].Ac_Nickname = HttpUtility.HtmlDecode(name);
            _ag[0].Ac_Phone = HttpUtility.HtmlDecode(phone);
            _ag[0].Ac_Pwd = HttpUtility.HtmlDecode(phone);
            db.Entry(_ag[0]).State = EntityState.Modified;
            db.SaveChanges();

            return "0";
        }

        [HttpGet]
        public string DeleteAccount(int id)
        {
            db.Accounts.Remove(db.Accounts.Find(id));
            db.SaveChanges();

            return "0";
        }

        //=======================================================================================================================================

        public PartialViewResult _PartialPageAccountGroupDeviceList(int? searchaccountgroupidb)
        {
            int _id = 0;
            if (null == searchaccountgroupidb)
            {
                _id = 1;//取巧//取默认区域数据//一定返回某个值
            }
            else
            {
                _id = searchaccountgroupidb.Value;
            }

            //ViewBag.areaid = searchareaid;
            //var _sev = from s in db.AccountGroupDevices where (s.AcountGroupID == _id) orderby s.ID select s;


            var _sev = from c in db.Devices
                       join o in db.AccountGroupDevices on c.ID equals o.DeviceID
                       where o.AcountGroupID == _id
                       select c;



            return PartialView(_sev.ToList());
        }




        //=======================================================================================================================================
        public JsonResult AccountGroupDeviceJsonTree()
        {

            // if (Session["userName"] == null)
            //     return Json("error", JsonRequestBehavior.AllowGet);

            // 按照区域排序
            List<Device> items = db.Devices.OrderBy(ss => ss.AreaID).ToList();
            //
            List<AccountGroupViewModels> hierarchicalItems = items
                .ToList().Select(c => new AccountGroupViewModels
                {
                    id = c.ID,
                    text = c.D_Name,
                    parent = 0
                }).ToList();
            //
            var s = hierarchicalItems.Select(a => new
            {
                a.id,
                a.text,
                a.children
            });

            JsonResult jsr = Json(s, JsonRequestBehavior.AllowGet);
            return jsr;
        }


        [HttpGet]
        public string AccountGroupDeviceEdit(int gid, string didarr)
        {
            if(gid < 1)
            {
                return "-1";
            }


            List<AccountGroupDevice> _d = (from m in db.AccountGroupDevices
                                           where (m.AcountGroupID == gid)
                                           select m).ToList();


            //取巧了，本来应该删掉已取消的，添加新增的
            //这里直接全删除
            //
            if (_d.Count() > 0)
            {
                for (int n = 0; n < _d.Count; n++)
                {
                    db.Entry(_d[n]).State = EntityState.Deleted;
                }
                db.SaveChanges();
            }


            //删除所有的设备
            if (didarr == "")
            {


                return "0";
            }

            //
            string[] sArray = didarr.Split('-');
            for(int i = 0 ; i < sArray.Length ; i++)
            {
                AccountGroupDevice _aag = new AccountGroupDevice()
                {
                    AcountGroupID = gid,
                    DeviceID = int.Parse(sArray[i]),
                };
                db.AccountGroupDevices.Add(_aag);
                db.SaveChanges();
            }


            //




            return "0";
        }

        [HttpGet]
        public string DeleteAccountGroupDevice(int gid , int did)
        {

            List<AccountGroupDevice> _d = (from m in db.AccountGroupDevices
                                           where ((m.AcountGroupID == gid) && (m.DeviceID == did))
                                           select m).ToList();

            if (_d.Count() > 0)
            {
                for (int n = 0; n < _d.Count; n++)
                {
                    db.Entry(_d[n]).State = EntityState.Deleted;
                }
                db.SaveChanges();
            }

            return "0";
        }


        protected override void Dispose(bool disposing)
        {
            if (disposing)
            {
                db.Dispose();
            }
            base.Dispose(disposing);
        }
    }
}
