import os
import json
from jinja2 import Template
from jinja2 import FileSystemLoader
from jinja2.environment import Environment
import btmesh_memsizes

class ElementData(object):

    def __init__(self, Name, Loc, NumS, NumV, SIGModels, VendorModels = None):
        self.Name = Name
        self.Loc = Loc
        self.NumS = NumS
        self.NumV = NumV
        self.SIGModels = SIGModels if SIGModels else []
        self.VendorModels = VendorModels if VendorModels else []

    def addSIGModel(self, model):
        self.SIGModels.append(model)

    def addVendorModel(self, model):
        self.VendorModels.append(model)

    def validate(self):
        if (self.NumS != len(self.SIGModels)):
            return "NumS=%u != len(SIGModels)=%u"%(self.NumS, len(self.SIGModels))
        if (self.NumV != len(self.VendorModels)):
            return "NumV=%u != len(VendorModels)=%u"%(self.NumV, len(self.VendorModels))
        return ""

class DCD(object):

    def __init__(self, CID, PID, VID, CRPL, Features, Elements = None):
        self.CID = CID
        self.PID = PID
        self.VID = VID
        self.CRPL = CRPL
        self.Features = Features
        self.Elements = Elements if Elements else []
        self.TotalModels = sum(len(elem.SIGModels) + len(elem.VendorModels) for elem in Elements)

class JSONMemoryConfig(object):

    def __init__(self, in_json):
        j = json.loads(in_json)['Memory Configuration']
        self.node_netkeys = int(j['Node Netkeys'], 0)
        self.node_appkeys = int(j['Node Appkeys'], 0)
        self.node_network_cache = int(j['Node Network Cache'], 0)
        self.node_vas = int(j['Node Virtual Addresses'], 0)
        self.trans_segtx = int(j['Transport Segmented TX'], 0)
        self.trans_segrx = int(j['Transport Segmented RX'], 0)
        self.model_appkey_bindings = int(j['Model Appkey Bindings'], 0)
        self.model_subscriptions = int(j['Model Subscriptions'], 0)
        self.gatt_proxy_connections = int(j['GATT Proxy Connections'], 0)
        self.gatt_tx_queue_size = int(j['GATT TX Queue Size'], 0)
        self.foundation_client_commands = int(j['Foundation Client Commands'], 0)
        self.provisioning_sessions = int(j['Provisioning Sessions'], 0)
        self.provisioned_devices = int(j['Provisioned Devices'], 0)
        self.provisioned_device_max_net_keys = int(j['Provisioned Device Max Netkeys'], 0)
        self.pstore_seq_write_interval = int(j['PStore SEQ Write Interval'], 0)
        self.node_max_friendships = int(j.get('Node Friendships', '0'), 0)
        try:
            self.node_friend_max_total_cache = int(j['Friend Max Total Cache'], 0)
            self.node_friend_max_single_cache = int(j['Friend Max Single Cache'], 0)
            self.node_friend_max_subscription_list = int(j['Friend Max Subscription list Size'], 0)
        except:
            self.node_friend_max_total_cache = 0
            self.node_friend_max_single_cache = 0
            self.node_friend_max_subscription_list = 0
        
class JSONDCD(object):

    def __init__(self, in_json):
        jsondata = json.loads(in_json)['Composition Data']
        CID = int(jsondata['CID'], 0)
        PID = int(jsondata['PID'], 0)
        VID = int(jsondata['VID'], 0)
        CRPL = int(jsondata['CRPL'], 0)
        Features = int(jsondata['Features'], 0)
        elements = []
        for elemdata in jsondata['Elements']:
            elem = ElementData(elemdata['Name'],
                    int(elemdata['Loc'], 0),
                    int(elemdata['NumS'], 0),
                    int(elemdata['NumV'], 0),
                    None,
                    None)
            # It's OK to have nonexisting model lists
            try:
                for i in range(0, len(elemdata['SIG Models']), 2):
                    elem.addSIGModel((int(elemdata['SIG Models'][i], 0), elemdata['SIG Models'][i+1]))
            except KeyError:
                pass
            try:
                for i in range(0, len(elemdata['Vendor Models']), 2):
                    elem.addVendorModel((int(elemdata['Vendor Models'][i], 0), elemdata['Vendor Models'][i+1]))
            except KeyError:
                pass

            if elem.validate() != "":
                raise Exception(elem.validate())

            elements.append(elem)

        self.dcd = DCD(CID, PID, VID, CRPL, Features, elements)

def calculate_memory_usage(dcd, memcfg, memsizes):
    total = 0
    total += len(dcd.Elements) * memsizes['mesh_element']
    total += dcd.TotalModels * (memsizes['model_base'] + memcfg.model_appkey_bindings * memsizes['model_per_app_binding'] +
            memcfg.model_subscriptions * memsizes['model_per_subscription'])
    total += memcfg.node_netkeys * memsizes['netkey']
    total += memcfg.node_appkeys * memsizes['appkey']
    #total += 1 * memsizes['devkey']
    total += memcfg.foundation_client_commands * memsizes['foundation_cmd']
    total += memcfg.node_network_cache * memsizes['net_cache']
    total += dcd.CRPL * memsizes['replay_entry']
    total += memcfg.trans_segtx * memsizes['seg_send']
    total += memcfg.trans_segrx * memsizes['seg_recv']
    total += memcfg.node_vas * memsizes['va']
    total += memcfg.provisioning_sessions * (memsizes['prov_session'] + memsizes['genprov_session'])
    #total += 1 + (dcd.Features & 0x1) * memsizes['prov_bearer']
    total += memcfg.gatt_proxy_connections * memsizes['mesh_gatt_conn']
    total += memcfg.gatt_tx_queue_size * memsizes['gatt_txq']
    total += memcfg.provisioned_devices * (memsizes['prv_ddb_entry_base'] + memcfg.provisioned_device_max_netkeys * memsizes['prv_ddb_entry_per_node_netkey'])
    return total

if __name__ == '__main__':
    import sys
    import argparse
    dcdparser = None
    memcfg = None

    parser = argparse.ArgumentParser(description='BLE Mesh Device Composition Data parser')
    parser.add_argument('--injson', dest='injson', type=argparse.FileType('r'), nargs=1,
                        help='JSON format input file')
    parser.add_argument('--outc', dest='outc', type=argparse.FileType('w'), nargs=1,
                        help='Filename for output DCD C file')
    parser.add_argument('--outh', dest='outh', type=argparse.FileType('w'), nargs=1,
                        help='Filename for output memory config C header file')
    parser.add_argument('--outjson', dest='outjson', type=argparse.FileType('w'), nargs=1,
                        help='Filename for output JSON')
    parser.add_argument('--memsizes', dest='memsizes', type=argparse.FileType('r'), nargs=1,
                        help='path/to/libble_mesh_memsizes.a if you want to calculate memory usage')

    args = parser.parse_args()
    if not args.memsizes:
        if not args.injson:
            print('No input specified!')
            exit(2)
        if not args.outjson and not args.outc and not args.outh:
            print('No output specified!')
            exit(3)

    if args.injson:
        contents = args.injson[0].read()
        memcfg = JSONMemoryConfig(contents)
        dcdparser = JSONDCD(contents)

    # This is a bit of a hack, but since we're calling external tools in the memory size parser,
    # it's easier to use the filename instead of the file pipe
    if args.memsizes and dcdparser:
        fname = args.memsizes[0].name
        args.memsizes[0].close()
        sizes = btmesh_memsizes.get_sizes(fname)
        print("BT Mesh heap usage: {}".format(calculate_memory_usage(dcdparser.dcd, memcfg, sizes)))

    env = Environment(trim_blocks=True, keep_trailing_newline=True)
    env.loader = FileSystemLoader(os.path.dirname(__file__))

    if args.outc:
        args.outc[0].write(env.get_template("templates/dcd.ct").render(
            CID=dcdparser.dcd.CID,
            PID=dcdparser.dcd.PID,
            VID=dcdparser.dcd.VID,
            CRPL=dcdparser.dcd.CRPL,
            Features=dcdparser.dcd.Features,
            Elements=dcdparser.dcd.Elements,
            TotalElements=len(dcdparser.dcd.Elements),
            TotalModels=dcdparser.dcd.TotalModels,
            MemCfg=memcfg,
            ProvBearers=1 + (dcdparser.dcd.Features & 0x1)))
        print("DCD written to {}".format(args.outc[0].name))
    if args.outh:
        args.outh[0].write(env.get_template("templates/mesh_app_memory_config.ht").render(
            CRPL=dcdparser.dcd.CRPL,
            TotalElements=len(dcdparser.dcd.Elements),
            TotalModels=dcdparser.dcd.TotalModels,
            MemCfg=memcfg,
            ProvBearers=1 + (dcdparser.dcd.Features & 0x1)))
        print("Memory config written to {}".format(args.outh[0].name))

