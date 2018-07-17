using NewPlatCenter.Models;
using NewPlatCenter.ViewModels;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.Mvc;
using PagedList;
using System.Data.Entity;

namespace NewPlatCenter.Controllers
{
    public class HomeController : Controller
    {
        private PlatCenterEntities db = new PlatCenterEntities();


        public ActionResult Index()
        {
            if (Session["userID"] == null)
                return RedirectToAction("Index", "Login");
            return View();
        }
        //=======================================================================================================================================
        [HttpGet]
        public string AddArea(int pid, string name)
        {
            if (pid < 1)
            {
                return "-1";
            }
            if (name.Length <= 0)
            {
                return "-2";
            }
            Area area = new Area()
            {
                Ar_Name = HttpUtility.HtmlDecode(name),
                Ar_Admin = 0,
                Ar_ParentID = pid
            };
            db.Areas.Add(area);
            db.SaveChanges();//EF插入的时候会自动返回并赋值给原来的实体对象。也就是这个对象savechange后，自增ID就有值了
            return area.ID.ToString();
        }


        [HttpGet]
        public string EditArea(int id, string name)
        {
            List<Area> _areas = (from m in db.Areas
                                 where ((m.ID == id))
                                 select m).ToList();
            if (1 != _areas.Count())
            {
                return "-1";
            }

            if (name.Length <= 0)
            {
                return "-2";
            }

            _areas[0].Ar_Name = HttpUtility.HtmlDecode(name);
            db.Entry(_areas[0]).State = EntityState.Modified;
            db.SaveChanges();

            return "0";
        }

        [HttpGet]
        public string DeleteArea(int id)
        {
            if (1 == id)
            {
                //不允许删除
                return "-1";
            }

            //如果下面有子项则不允许删除

            List<Area> _z = (from m in db.Areas
                               where (m.Ar_ParentID == id)
                               select m).ToList();
            if (_z.Count() > 0)
            {
                return "-2";
            }


            //下面有设备
            List<Device> _d = (from m in db.Devices
                               where (m.AreaID == id)
                               select m).ToList();
            if (_d.Count() > 0)
            {
                for (int n = 0; n < _d.Count; n++)
                {
                    _d[n].AreaID = 1;//将设备挂载到默认区域,取巧，默认区域一定是1
                    db.Entry(_d[n]).State = EntityState.Modified;


                    //------删除用户对应得设备
                    List<AccountGroupDevice> _dag = (from m in db.AccountGroupDevices
                                                     where (m.DeviceID == _d[n].ID)
                                                   select m).ToList();
                    if (_dag.Count() > 0)
                    {
                        for (int i = 0; i < _dag.Count; i++)
                        {
                            db.Entry(_dag[i]).State = EntityState.Deleted;
                        }
                    }

                }
            }
            db.Areas.Remove(db.Areas.Find(id));
            db.SaveChanges();

            return "0";
        }






        //=======================================================================================================================================
        [HttpGet]
        public string AddDevice(int areaid, int disk, int serial, string name, int mainsub,int type,string ctx)
        {
            if (areaid < 1)
            {
                return "-1";
            }
            if (disk < 1)
            {
                return "-2";
            }
            if (serial < 0)
            {
                return "-3";
            }
            if (name.Length <= 0)
            {
                return "-4";
            }

            Device _device = new Device()
            {
                AreaID = areaid,
                CoreServerID = 0,
                LiveServerID = 0,
                ClusterServerID = 0,
                D_Disk = disk,
                D_Name = HttpUtility.HtmlDecode(name),
                D_Serial = serial,
                D_Stream_Main_Sub = mainsub,
                D_Type = type,
                D_Content = ctx,
                D_Browse_Num = 0,
                D_Is_Online = 0,
                D_Date = DateTime.Now
            };
            db.Devices.Add(_device);
            db.SaveChanges();//EF插入的时候会自动返回并赋值给原来的实体对象。也就是这个对象savechange后，自增ID就有值了
            return _device.ID.ToString();
        }

        [HttpGet]
        public string EditDevice(int id, int disk, int serial, string name, int mainsub,int type,string ctx)
        {
            List<Device> _d = (from m in db.Devices
                               where ((m.ID == id))
                               select m).ToList();
            if (1 != _d.Count())
            {
                return "-1";
            }

            if (disk < 1)
            {
                return "-2";
            }
            if (serial < 0)
            {
                return "-3";
            }
            if (name.Length <= 0)
            {
                return "-4";
            }

            _d[0].D_Disk = disk;
            _d[0].D_Name = HttpUtility.HtmlDecode(name);
            _d[0].D_Serial = serial;
            _d[0].D_Stream_Main_Sub = mainsub;
            _d[0].D_Type = type;
            _d[0].D_Content = ctx;
            db.Entry(_d[0]).State = EntityState.Modified;
            db.SaveChanges();

            return "0";
        }


        [HttpGet]
        public string EditDeviceArea(int id, int areaid)
        {
            List<Device> _d = (from m in db.Devices
                               where ((m.ID == id))
                               select m).ToList();
            if (1 != _d.Count())
            {
                return "-1";
            }

            if (areaid < 1)
            {
                return "-2";
            }

            _d[0].AreaID = areaid;
            db.Entry(_d[0]).State = EntityState.Modified;
            db.SaveChanges();

            return "0";
        }



        [HttpGet]
        public string DeleteDevice(int id)
        {
            if (id <= 0)
            {
                return "-1";
            }

            //将用户组的设备删除
            List<AccountGroupDevice> _d = (from m in db.AccountGroupDevices
                                           where (m.DeviceID == id)
                                           select m).ToList();
            if (_d.Count() > 0)
            {
                for (int n = 0; n < _d.Count; n++)
                {
                    db.Entry(_d[n]).State = EntityState.Deleted;
                }
                db.SaveChanges();
            }

            db.Devices.Remove(db.Devices.Find(id));
            db.SaveChanges();

            return "0";
        }
        //=======================================================================================================================================

        public PartialViewResult _PartialPageDeviceList(int? searchareaid)
        {
            int _areaid = 0;
            if (null == searchareaid)
            {
                _areaid = 1;//取巧//取默认区域数据//一定返回某个值
            }
            else
            {
                _areaid = searchareaid.Value;
            }

            //ViewBag.areaid = searchareaid;
            var _sev = from s in db.Devices where (s.AreaID == _areaid) orderby s.ID select s;
            return PartialView(_sev.ToList());
        }
        public ActionResult MoveDevice()
        {
            //
            return View();
        }



        public ActionResult About()
        {
            return View();
        }
        public ActionResult Help()
        {
            return View();
        }



        public JsonResult AreasJsonTree()
        {

            // if (Session["userName"] == null)
            //     return Json("error", JsonRequestBehavior.AllowGet);



            List<Area> items = null;
            items = db.Areas.OrderBy(ss => ss.ID).ToList();


            Action<AreasViewModels> SetChildren = null;
            SetChildren = parent =>
            {
                parent.children = items
                    .Where(childItem => childItem.Ar_ParentID == parent.id)
                    .ToList().Select(c => new AreasViewModels
                    {
                        id = c.ID,
                        text = c.Ar_Name,
                        parent = c.Ar_ParentID
                    }).ToList();

                //Recursively call the SerChildren method for each child.
                parent.children.ForEach(SetChildren);

            };

            //Initialize the hierarchical list to root level item
            List<AreasViewModels> hierarchicalItems = items
                .Where(rootItem => rootItem.Ar_ParentID == 0)
                .ToList().Select(c => new AreasViewModels
                {
                    id = c.ID,
                    text = c.Ar_Name,
                    parent = c.Ar_ParentID
                }).ToList();

            hierarchicalItems.ForEach(SetChildren);


            var s = hierarchicalItems.Select(a => new
            {
                a.id,
                a.text,
                a.children
            });

            JsonResult jsr = Json(s, JsonRequestBehavior.AllowGet);
            return jsr;
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