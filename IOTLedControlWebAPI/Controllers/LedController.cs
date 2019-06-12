using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using IOTLedControlWebAPI.Models;
using IOTLedControlWebAPI.Services;
using Microsoft.AspNetCore.Mvc;
using Newtonsoft.Json;

namespace IOTLedControlWebAPI.Controllers
{
    [Route("api/[controller]")]
    [ApiController]
    public class LedController : Controller
    {
        private NotificationsMessageHandler _notificationsMessageHandler { get; set; }

        public LedController(NotificationsMessageHandler notificationsMessageHandler)
        {
            _notificationsMessageHandler = notificationsMessageHandler;
        }

        [HttpPost("send")]
        public async Task SendMessage([FromBody]RGBData data)
        {
            if(data != null)
            {
                await _notificationsMessageHandler.SendMessageToAllAsync(JsonConvert.SerializeObject(data));
            }
            
        }
    }
}