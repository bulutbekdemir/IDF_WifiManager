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
function AddNew(ssid, password) {
  const newDiv = document.createElement("button");
	newDiv.classList.add("btn");
	newDiv.innerHTML = "SSID: " + ssid + " Password: " + password;
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
       
        log(response);
          
    }
}