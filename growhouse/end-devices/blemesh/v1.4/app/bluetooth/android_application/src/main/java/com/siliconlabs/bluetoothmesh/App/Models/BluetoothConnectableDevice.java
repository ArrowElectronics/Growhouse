/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Models;

import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothProfile;
import android.bluetooth.le.ScanRecord;
import android.bluetooth.le.ScanResult;
import android.content.Context;
import android.os.Handler;
import android.os.Looper;
import android.os.ParcelUuid;
import android.util.Log;

import com.siliconlab.bluetoothmesh.adk.connectable_device.ConnectableDevice;
import com.siliconlab.bluetoothmesh.adk.connectable_device.ConnectableDeviceSubscriptionCallback;
import com.siliconlab.bluetoothmesh.adk.connectable_device.ConnectableDeviceWriteCallback;

import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.List;
import java.util.Objects;
import java.util.UUID;

public class BluetoothConnectableDevice extends ConnectableDevice {
    private static final String TAG = BluetoothConnectableDevice.class.getSimpleName();
    private Context context;
    private BluetoothGatt bluetoothGatt;

    private BluetoothDevice bluetoothDevice;
    private ScanResult scanResult;
    private int mtuSize;
    private String address;
    private byte[] advertisementData;

    private Handler mainHandler = new Handler(Looper.getMainLooper());

    private final List<DeviceConnectionCallback> deviceConnectionCallbacks = new ArrayList<>();

    public void addDeviceConnectionCallback(DeviceConnectionCallback deviceConnectionCallback) {
        synchronized (deviceConnectionCallbacks) {
            deviceConnectionCallbacks.add(deviceConnectionCallback);
        }
    }

    public void removeDeviceConnectionCallback(DeviceConnectionCallback deviceConnectionCallback) {
        synchronized (deviceConnectionCallbacks) {
            deviceConnectionCallbacks.remove(deviceConnectionCallback);
        }
    }

    private void notifyConnectionState(boolean connected) {
        synchronized (deviceConnectionCallbacks) {
            for (DeviceConnectionCallback callback : deviceConnectionCallbacks) {
                if (connected) {
                    callback.onConnectedToDevice();
                } else {
                    callback.onDisconnectedFromDevice();
                }
            }
        }
    }

    public BluetoothConnectableDevice(Context context, ScanResult scanResult) {
        this.context = context;
        this.bluetoothDevice = scanResult.getDevice();
        this.advertisementData = Objects.requireNonNull(scanResult.getScanRecord()).getBytes();
        this.address = bluetoothDevice.getAddress();
        this.scanResult = scanResult;
    }

    @Override
    public String getName() {
        return bluetoothDevice.getName();
    }

    @Override
    public void connect() {
        Log.d(TAG, "connect");
        mainThreadChecker();
        bluetoothGatt = bluetoothDevice.connectGatt(context, false, bluetoothGattCallback, BluetoothDevice.TRANSPORT_LE);
    }

    @Override
    public void disconnect() {
        Log.d(TAG, "disconnect");
        mainThreadChecker();
        if (bluetoothGatt != null) {
            refreshDeviceCache(bluetoothGatt);
            bluetoothGatt.disconnect();
        }
    }

    @Override
    public byte[] getAdvertisementData() {
        return advertisementData;
    }

    @Override
    public int getMTU() {
        Log.d(TAG, "getMTU " + mtuSize);
        return mtuSize;
    }

    @Override
    public byte[] getServiceData(UUID service) {
        byte[] data = null;

        ScanRecord scanRecord = scanResult.getScanRecord();
        if (scanRecord != null) {
            data = scanRecord.getServiceData(new ParcelUuid(service));
        }

        return data;
    }

    @Override
    public boolean hasService(UUID service) {
        Log.d(TAG, "hasService " + service);
        ScanRecord scanRecord = scanResult.getScanRecord();
        if (scanRecord == null) {
            return false;
        }
        List<ParcelUuid> serviceUuids = scanRecord.getServiceUuids();
        if (serviceUuids == null) {
            return false;
        }

        return serviceUuids.contains(new ParcelUuid(service));
    }

    @Override
    public void writeData(UUID service, UUID characteristic, byte[] data, ConnectableDeviceWriteCallback connectableDeviceWriteCallback) {
        mainThreadChecker();
        if (bluetoothGatt == null) {
            connectableDeviceWriteCallback.onFailed(service, characteristic);
            return;
        }
        BluetoothGattService bluetoothGattService = bluetoothGatt.getService(service);
        if (bluetoothGattService == null) {
            connectableDeviceWriteCallback.onFailed(service, characteristic);
            return;
        }
        BluetoothGattCharacteristic bluetoothGattCharacteristic = bluetoothGattService.getCharacteristic(characteristic);
        bluetoothGattCharacteristic.setValue(data);
        bluetoothGattCharacteristic.setWriteType(BluetoothGattCharacteristic.WRITE_TYPE_NO_RESPONSE);
        if (bluetoothGatt.writeCharacteristic(bluetoothGattCharacteristic)) {
            connectableDeviceWriteCallback.onWrite(service, characteristic);
        } else {
            connectableDeviceWriteCallback.onFailed(service, characteristic);
        }
    }


    @Override
    public void subscribe(final UUID service, final UUID characteristic, final ConnectableDeviceSubscriptionCallback connectableDeviceSubscriptionCallback) {
        Log.d(TAG, "subscribe service : " + service + " characteristic: " + characteristic);
        mainThreadChecker();

        for (BluetoothGattService bluetoothGattService : bluetoothGatt.getServices()) {
            Log.d(TAG, "subscribe found service " + bluetoothGattService.getUuid());
        }
        if (bluetoothGatt == null) {
            connectableDeviceSubscriptionCallback.onFail(service, characteristic);
            return;
        }
        BluetoothGattService bluetoothGattService = bluetoothGatt.getService(service);
        if (bluetoothGattService == null) {
            connectableDeviceSubscriptionCallback.onFail(service, characteristic);
            return;
        }
        BluetoothGattCharacteristic bluetoothGattCharacteristic = bluetoothGattService.getCharacteristic(characteristic);
        if (!bluetoothGatt.setCharacteristicNotification(bluetoothGattCharacteristic, true)) {
            connectableDeviceSubscriptionCallback.onFail(service, characteristic);
            return;
        }
        if (bluetoothGattCharacteristic.getDescriptors().size() != 1) {
            connectableDeviceSubscriptionCallback.onFail(service, characteristic);
            return;
        }
        BluetoothGattDescriptor bluetoothGattDescriptor = bluetoothGattCharacteristic.getDescriptors().get(0);
        bluetoothGattDescriptor.setValue(BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE);
        if (!bluetoothGatt.writeDescriptor(bluetoothGattDescriptor)) {
            connectableDeviceSubscriptionCallback.onFail(service, characteristic);
            return;
        }
        mainHandler.postDelayed(new Runnable() {
            @Override
            public void run() {
                connectableDeviceSubscriptionCallback.onSuccess(service, characteristic);
            }
        }, 200);
    }

    private BluetoothGattCallback bluetoothGattCallback = new BluetoothGattCallback() {
        private int connectionAttempts = 0;

        private void changeMtu(final BluetoothGatt bluetoothGatt) {
            mainHandler.post(new Runnable() {
                @Override
                public void run() {
                    for (int i = 0; i != 3; i++) {
                        if (bluetoothGatt.requestMtu(512)) {
                            return;
                        }
                        Log.d(TAG, "retry request mtu i:" + i);
                    }
                }
            });
        }

        private void discoverServices(final BluetoothGatt bluetoothGatt) {
            mainHandler.post(new Runnable() {
                @Override
                public void run() {
                    for (int i = 0; i != 3; i++) {
                        if (bluetoothGatt.discoverServices()) {
                            return;
                        }
                        Log.d(TAG, "retry discover services i:" + i);
                    }
                }
            });
        }

        @Override
        public void onConnectionStateChange(final BluetoothGatt gatt, final int status, final int newState) {
            super.onConnectionStateChange(gatt, status, newState);
            Log.d(TAG, "onConnectionStateChange : status: " + status + ", newstate: " + newState);

            switch (status) {
                case BluetoothGatt.GATT_SUCCESS:
                    connectionAttempts = 0;

                    if (newState == BluetoothProfile.STATE_CONNECTED) {
                        changeMtu(gatt);
                    } else {
                        bluetoothGatt.close();
                        onDisconnected();
                        notifyConnectionState(false);
                    }
                    break;
                default:
                    if (!isConnected()) {
                        Log.d(TAG, "connect connectionAttempts=" + connectionAttempts);
                        connectionAttempts++;
                        if (connectionAttempts <= 3) {
                            mainHandler.post(new Runnable() {
                                @Override
                                public void run() {
                                    gatt.close();
                                    connect();
                                }
                            });
                        } else {
                            connectionAttempts = 0;
                        }
                    } else {
                        connectionAttempts = 0;

                        onDisconnected();
                        notifyConnectionState(false);
                    }
            }
        }

        @Override
        public void onMtuChanged(final BluetoothGatt bluetoothGatt, final int mtu, final int status) {
            super.onMtuChanged(bluetoothGatt, mtu, status);
            Log.d(TAG, "onMtuChanged : status: " + status + ", mtu: " + mtu);
            if (status == BluetoothGatt.GATT_SUCCESS) {
                mtuSize = mtu;
                discoverServices(bluetoothGatt);
            } else {
                changeMtu(bluetoothGatt);
            }
        }

        @Override
        public void onServicesDiscovered(final BluetoothGatt bluetoothGatt, int status) {
            super.onServicesDiscovered(bluetoothGatt, status);
            Log.d(TAG, "onServicesDiscovered : status: " + status);
            if (status == BluetoothGatt.GATT_SUCCESS) {
                mainHandler.postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        onConnected();
                        notifyConnectionState(true);
                    }
                }, 1000);
            } else {
                discoverServices(bluetoothGatt);
            }
        }

        @Override
        public void onCharacteristicChanged(BluetoothGatt bluetoothGatt, BluetoothGattCharacteristic bluetoothGattCharacteristic) {
            Log.d(TAG, "onCharacteristicChanged : bluetoothGattCharacteristic: " + bluetoothGattCharacteristic.getUuid());
            updateData(bluetoothGattCharacteristic.getService().getUuid(), bluetoothGattCharacteristic.getUuid(), bluetoothGattCharacteristic.getValue());
        }
    };

    public String getAddress() {
        return address;
    }

    private void refreshDeviceCache(BluetoothGatt gatt) {
        try {
            Method refreshmethod = gatt.getClass().getMethod("refresh");
            boolean result = (boolean) (Boolean) refreshmethod.invoke(gatt, new Object[0]);
            Log.d(TAG, "refreshDeviceCache " + result);
        } catch (Exception localException) {
            Log.e(TAG, "An exception occured while refreshing device");
        }
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        BluetoothConnectableDevice that = (BluetoothConnectableDevice) o;
        return Objects.equals(scanResult, that.scanResult);
    }

    @Override
    public int hashCode() {
        return Objects.hash(scanResult);
    }

    private void mainThreadChecker() {
        if (!Looper.getMainLooper().equals(Looper.myLooper())) {
            throw new RuntimeException("Not on the main thread.");
        }
    }

    public interface DeviceConnectionCallback {
        void onConnectedToDevice();

        void onDisconnectedFromDevice();
    }
}
