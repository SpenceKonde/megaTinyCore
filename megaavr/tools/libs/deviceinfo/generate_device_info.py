import os
import tempfile
from string import Template
from . import harvest
from . import artifact_fetcher as AF



DEVICE_INFO_TEMPLATE = '''
"""
Required device info for the $device_name devices
The following data was collected from device pack $pack_info
"""

from pymcuprog.deviceinfo.eraseflags import ChiperaseEffect

DEVICE_INFO = {
$device_data    
}
'''

def extract_device_name(device_atdf_path):
    harvest_data, _ = harvest.harvest_from_file(device_atdf_path)
    name_container_ind = harvest_data.find("'name'") + len("'name'")
    name_start_ind = harvest_data[name_container_ind:].find("'") + name_container_ind + 1
    name_end_ind = harvest_data[name_start_ind:].find("'") + name_start_ind
    name = harvest_data[name_start_ind:name_end_ind]
    return name


def add_new_updi_device(device_atdf_path, device_pack_source_str):
    """
    param device_atdf_path: Path to .atdf file of the device you wish to add
    param device_pack_source_str: Reference string to the source of the DFP from which the .atdf file was extracted
    """
    harvest_data, sign_success = harvest.harvest_from_file(device_atdf_path)
    if not "'interface': 'UPDI'" in harvest_data:
        return
    device_name = extract_device_name(device_atdf_path)
    if sign_success is False:
        print("Reading device signature for {} failed, this needs to be added manually.".format(device_name))
    new_device_template = Template(DEVICE_INFO_TEMPLATE)
    new_device_script = new_device_template.substitute(device_name=device_name, device_data=harvest_data,
                                                       pack_info=device_pack_source_str)

    f = open(os.getcwd().replace("\\", "/")+"/devices/" + device_name + ".py", "w")
    f.write(new_device_script)
    f.close()

def update_updi_devices(repo):
    """
    param repo: Select if you'd like to update devices from "stable", "release" or "continuous" artifactory repository
    """

    for ind in range(len(AF.ARTIFACT_SERVERS_MCHP)):
        temp_dir = tempfile.TemporaryDirectory()
        pack_info = ""

        print("Fetching {} device pack. This may take a while.".format(AF.ARTIFACT_SERVERS_MCHP[ind]['serves_device']))
        try:
            pack_info = AF.fetch_device_atpack(AF.ARTIFACT_SERVERS_MCHP[ind], repo, temp_dir.name)
        except:
            print("Failed to fetch {} device pack from Microchip artifact server. Trying Atmel server..."
                .format(AF.ARTIFACT_SERVERS_ATMEL[ind]['serves_device']))
            try:
                pack_info = AF.fetch_device_atpack(AF.ARTIFACT_SERVERS_ATMEL[ind], repo, temp_dir.name)
            except:
                print("Failed to fetch {} device pack".format(AF.ARTIFACT_SERVERS_ATMEL[ind]['serves_device']))

        devices = os.listdir(temp_dir.name.replace("\\", "/")+"/atdf")

        for j in range(len(devices)):
            add_new_updi_device(temp_dir.name.replace("\\", "/") + "/atdf/" + devices[j], pack_info)

        print("Success.")

update_updi_devices('stable')
