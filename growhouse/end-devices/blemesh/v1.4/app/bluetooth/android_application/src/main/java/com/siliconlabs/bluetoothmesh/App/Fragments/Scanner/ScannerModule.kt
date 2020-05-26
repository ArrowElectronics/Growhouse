/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Fragments.Scanner

import android.content.Context
import com.siliconlabs.bluetoothmesh.App.Logic.BluetoothScanner
import com.siliconlabs.bluetoothmesh.App.Logic.BluetoothStateReceiver
import com.siliconlabs.bluetoothmesh.App.Logic.MeshLogic
import com.siliconlabs.bluetoothmesh.App.Models.ProvisioningModel
import dagger.Module
import dagger.Provides

/**
 * @author Comarch S.A.
 */
@Module
class ScannerModule {

    @Provides
    fun provideScannerView(scannerFragment: ScannerFragment): ScannerView {
        return scannerFragment
    }

    @Provides
    fun provideScannerPresenter(scannerView: ScannerView,context: Context, meshLogic: MeshLogic, provisioningModel: ProvisioningModel, bluetoothLeScanLogic: BluetoothScanner, bluetoothStateReceiver: BluetoothStateReceiver): ScannerPresenter {
        return ScannerPresenter(scannerView,context, meshLogic, provisioningModel, bluetoothLeScanLogic, bluetoothStateReceiver)
    }
}