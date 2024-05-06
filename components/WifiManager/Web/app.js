const BtnAdd = document.getElementById("set_btn");
const DivContainer = document.getElementById("network_select_buttons");

BtnAdd.addEventListener("click",AddNew);

/*!
 * Initialize functions here.
 */
$(document).ready(function(){
	getWifiNetworks();
});  

/*!
* Add new button.
*/
function AddNew(ssid, authmode, rssi) {
  const newDiv = document.createElement("button");
	newDiv.classList.add("btn");
	newDiv.innerHTML = "SSID: " + ssid + " Authmode: " + authmode + " RSSI: " + rssi;
  console.log("add");
  DivContainer.appendChild(newDiv);
}

/*!
* Gets the wifi networks.
*/
function getWifiNetworks()
{
    var xhr = new XMLHttpRequest();
    var requestURL = "/scannedWifiNetworks";
    xhr.open('POST', requestURL, false);
    xhr.send('scannedWifiNetworks');

    if(xhr.readyState == 4 && xhr.status == 200)
    {
        var response = JSON.parse(xhr.responseText);

        getWifiNetworksList(response.ap_count);

        //AddNew(response.status, response.ap_count);
        //console.log(response);        
    }
}

/*!
* Gets the wifi networks list
*/
function getWifiNetworksList(list_size)
{
  var xhr = new XMLHttpRequest();
  var requestURL = "/listofScannedWifiNetworks";
  xhr.open('POST', requestURL, false);
  xhr.send('ListscanWifiNetworks');

  if(xhr.readyState == 4 && xhr.status == 200)
  {
    var response = JSON.parse(xhr.responseText);

    console.log(response);

    for(var i = 0; i < list_size; i++)
    {
      AddNew(response.ap_records[i].ssid, response.ap_records[i].authmode, response.ap_records[i].rssi);
    }

  }

}