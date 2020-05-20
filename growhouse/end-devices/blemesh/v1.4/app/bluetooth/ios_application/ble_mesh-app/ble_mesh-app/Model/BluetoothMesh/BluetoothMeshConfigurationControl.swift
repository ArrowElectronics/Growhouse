//
//  BluetoothMeshConfigurationControl.swift
//  ble_mesh-app
//
//  Created by Arkadiusz Marszałek on 16/01/2019.
//  Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
//

import BluetoothMesh

class BluetoothMeshConfigurationControl {
    
    typealias SetCallback = (_ error: Error?) -> ()
    typealias GetProxyCallback = (_ enabled: Bool?, _ error: Error?) -> ()
    typealias GetRelayCallback = (_ enabled: Bool?, _ retransmissionCount: UInt8?, _ retransmissionInterval: UInt8?, _ error: Error?) -> ()
    typealias GetFriendCallback = (_ enabled: Bool?, _ error: Error?) -> ()
    typealias GetNodeIdentityCallback = (_ enabled: Bool?, _ error: Error?) -> ()
    typealias GetRetransmissionConfigurationCallback = (_ retransmissionCount: UInt8?, _ retransmissionInterval: UInt8?, _ error: Error?) -> ()
    typealias GetDeviceCompositionDataCallback = (_ dcd: SBMDeviceCompositionData?, _ elements: [SBMElement]?, _ error: Error?) -> ()
    typealias FactoryResetCallback = (_ node: SBMNode, _ error: Error?) -> ()
    
    func setProxy(_ enabled: Bool, for node: SBMNode, completion: SetCallback? = nil) {
        Logger.log(message: "Set proxy: \(enabled) for node: \(node.name)")
        SBMConfigurationControl(node: node).setProxy(enabled, with: { node, enabled in
            Logger.log(message: "Success setting proxy to: \(enabled) for node: \(node.name)")
            completion?(nil)
        }, errorCallback: { node, error in
            Logger.log(message: "Error: \(error.localizedDescription), while setting proxy for node: \(node.name)")
            completion?(error)
        })
    }
    
    func setRelay(_ enabled: Bool, retransmissionCount: UInt8, retransmissionInterval: UInt8, for node: SBMNode, completion: SetCallback? = nil) {
        Logger.log(message: "Set relay: \(enabled), retransmissionCount: \(retransmissionCount), retransmissionInterval: \(retransmissionInterval) for node: \(node.name)")
        SBMConfigurationControl(node: node).setRelay(enabled, retransmissionCount: retransmissionCount, retransmissionInterval: retransmissionInterval, with: { node, enabled, retransmissionCount, retransmissionInterval in
            Logger.log(message: "Success setting relay to: \(enabled), retransmissionCount: \(retransmissionCount), retransmissionInterval: \(retransmissionInterval) for node: \(node.name)")
            completion?(nil)
        }, errorCallback: { node, error in
            Logger.log(message: "Error: \(error.localizedDescription), while setting relay for node: \(node.name)")
            completion?(error)
        })
    }
    
    func setFriend(_ enabled: Bool, for node: SBMNode, completion: SetCallback? = nil) {
        Logger.log(message: "Set friend: \(enabled) for node: \(node.name)")
        SBMConfigurationControl(node: node).setFriend(enabled, with: { node, enabled in
            Logger.log(message: "Success setting friend to: \(enabled) for node: \(node.name)")
            completion?(nil)
        }, errorCallback: { node, error in
            Logger.log(message: "Error: \(error.localizedDescription), while setting friend for node: \(node.name)")
            completion?(error)
        })
    }
    
    func setNodeIdentity(_ enabled: Bool, for node: SBMNode, in subnet: SBMSubnet, completion: SetCallback? = nil) {
        Logger.log(message: "Set node identity: \(enabled) for node: \(node.name)")
        SBMConfigurationControl(node: node).setNodeIdentity(subnet, enabled: enabled, with: { node, enabled, subnet in
            Logger.log(message: "Success setting node identity to: \(enabled) for node: \(node.name) in subnet: \(subnet.name)")
            completion?(nil)
        }, errorCallback: { node, error in
            Logger.log(message: "Error: \(error.localizedDescription), while setting node identity for node: \(node.name)")
            completion?(error)
        })
    }
    
    func setRetransmissionConfiguration(for node: SBMNode, retransmissionCount: UInt8, retransmissionInterval: UInt8, completion: SetCallback? = nil) {
        SBMConfigurationControl(node: node).setRetransmissionConfiguration(retransmissionCount, retransmissionInterval: retransmissionInterval, with: { node, count, interval in
            Logger.log(message: "Success setting retransmission count: \(count) and interval: \(interval) for node: \(node.name)")
            completion?(nil)
        }, errorCallback: { node, error in
            Logger.log(message: "Error: \(error.localizedDescription), while setting retransmission configuration for node: \(node.name)")
            completion?(error)
        })
    }
    
    func setHeartbeatPublication(for node: SBMNode,
                                 publicationAddress: UInt16,
                                 publishCountLogarithm: UInt8,
                                 publishPeriodLogarithm: UInt8,
                                 timeToLive: UInt8,
                                 features: UInt16,
                                 publishCryptoSubnetKeyIndex: UInt16) {
        SBMConfigurationControl(node: node).setHeartbeatPublication(publicationAddress,
                                                                    publishCountLogarithm: publishCountLogarithm,
                                                                    publishPeriodLogarithm: publishPeriodLogarithm,
                                                                    timeToLive: timeToLive,
                                                                    features: features,
                                                                    publishCryptoSubnetKeyIndex: publishCryptoSubnetKeyIndex,
                                                                    with: { _, _, _, _, _, _, _ in
                                                                        Logger.log(message: "Success setting heartbeat publication")
                                                                    }, errorCallback: { node, error in
                                                                        Logger.log(message: "Error: \(error.localizedDescription), while setting heartbeat publication for node: \(node.name)")
                                                                    })
    }
    
    func setHeartbeatSubscription(for node: SBMNode,
                                  sourceAddress: UInt16,
                                  destinationAddress: UInt16,
                                  period: UInt16) {
        SBMConfigurationControl(node: node).setHeartbeatSubscription(sourceAddress,
                                                                     destinationAddress: destinationAddress,
                                                                     period: period,
                                                                     with: { _, _, _, _, _, _, _ in
                                                                        Logger.log(message: "Success setting heartbeat subscription")
                                                                     }, errorCallback: { node, error in
                                                                        Logger.log(message: "Error: \(error.localizedDescription), while setting heartbeat subscription for node: \(node.name)")
                                                                     })
    }
    
    func getProxy(for node: SBMNode, completion: GetProxyCallback? = nil) {
        Logger.log(message: "Get proxy for node: \(node.name)")
        SBMConfigurationControl(node: node).checkProxyStatus({ node, enabled in
            Logger.log(message: "Success getting proxy: \(enabled) for node: \(node.name)")
            completion?(enabled, nil)
        }, errorCallback: { node, error in
            Logger.log(message: "Error: \(error.localizedDescription), while getting proxy for node: \(node.name)")
            completion?(nil, error)
        })
    }
    
    func getRelay(for node: SBMNode, completion: GetRelayCallback? = nil) {
        Logger.log(message: "Get relay for node: \(node.name)")
        SBMConfigurationControl(node: node).checkRelayStatus({ node, enabled, retransmissionCount, retransmissionInterval in
            Logger.log(message: "Success getting relay: \(enabled), retransmissionCount: \(retransmissionCount), retransmissionInterval: \(retransmissionInterval) for node: \(node.name)")
            completion?(enabled, retransmissionCount, retransmissionInterval, nil)
        }, errorCallback: { node, error in
            Logger.log(message: "Error: \(error.localizedDescription), while getting relay for node: \(node.name)")
            completion?(nil, nil, nil, error)
        })
    }
    
    func getFriend(for node: SBMNode, completion: GetFriendCallback? = nil) {
        Logger.log(message: "Get friend for node: \(node.name)")
        SBMConfigurationControl(node: node).checkFriendStatus({ node, enabled in
            Logger.log(message: "Success getting friend: \(enabled) for node: \(node.name)")
            completion?(enabled, nil)
        }, errorCallback: { node, error in
            Logger.log(message: "Error: \(error.localizedDescription), while getting friend for node: \(node.name)")
            completion?(nil, error)
        })
    }
    
    func getNodeIdentity(for node: SBMNode, in subnet: SBMSubnet, completion: GetNodeIdentityCallback? = nil) {
        Logger.log(message: "Get node identity for node: \(node.name)")
        SBMConfigurationControl(node: node).checkNodeIdentityStatus(subnet, successCallback: { node, enabled, _ in
            Logger.log(message: "Success getting node identity: \(enabled) for node: \(node.name)")
            completion?(enabled, nil)
        }, errorCallback: { node, error in
            Logger.log(message: "Error: \(error.localizedDescription), while getting node identity for node: \(node.name)")
            completion?(nil, error)
        })
    }
    
    func getRetransmissionConfiguration(for node: SBMNode, completion: GetRetransmissionConfigurationCallback? = nil) {
        Logger.log(message: "Get retransmission configuration for node: \(node.name)")
        SBMConfigurationControl(node: node).checkRetransmissionConfigurationStatus({ node, count, interval in
            Logger.log(message: "Success getting retransmission configuration count: \(count) and interval: \(interval) for node: \(node.name)")
            completion?(count, interval, nil)
        }, errorCallback: { node, error in
            Logger.log(message: "Error: \(error.localizedDescription), while getting retransmission configuration for node: \(node.name)")
            completion?(nil, nil, error)
        })
    }
    
    func getHeartbeatPublication(for node: SBMNode) {
        SBMConfigurationControl(node: node).checkHeartbeatPublicationStatus({ _, _, _, _, _, _, _ in
            Logger.log(message: "Success getting heartbeat publication")
        }, errorCallback: { node, error in
            Logger.log(message: "Error: \(error.localizedDescription), while getting heartbeat publication for node: \(node.name)")
        })
    }
    
    func getHeartbeatSubscription(for node: SBMNode) {
        SBMConfigurationControl(node: node).checkHeartbeatSubscriptionStatus({ _, _, _, _, _, _, _ in
            Logger.log(message: "Success getting heartbeat subscription")
        }, errorCallback: { node, error in
            Logger.log(message: "Error: \(error.localizedDescription), while getting heartbeat subscription for node: \(node.name)")
        })
    }
    
    func getDeviceCompositionData(for node: SBMNode, completion: GetDeviceCompositionDataCallback? = nil) {
        Logger.log(message: "Get dcd for node: \(node.name)")
        SBMConfigurationControl(node: node).getDeviceCompositionData({ _, dcd, elements in
            Logger.log(message: "Success getting dcd: \(dcd), elements: \(elements) for node: \(node.name)")
            completion?(dcd, elements, nil)
        }, errorCallback: { _, error in
            Logger.log(message: "Error: \(error.localizedDescription), while getting node identity for node: \(node.name)")
            completion?(nil, nil, error)
        })
    }
    
    func factoryReset(for node: SBMNode, completion: FactoryResetCallback? = nil) {
        Logger.log(message: "Reset node: \(node.name)")
        SBMConfigurationControl(node: node).factoryReset({ node in
            Logger.log(message: "Success reset node: \(node.name)")
            NotificationCenter.default.post(name: BluetoothMeshNotifications.NodesUpdated, object: nil)
            completion?(node, nil)
        }) { node, error in
            Logger.log(message: "Error: \(error.localizedDescription), while resetting node: \(node.name)")
            NotificationCenter.default.post(name: BluetoothMeshNotifications.NodesUpdated, object: nil)
            let isExpectedError = (error as NSError).code == 385
            completion?(node, isExpectedError ? nil : error)
        }
    }
}
