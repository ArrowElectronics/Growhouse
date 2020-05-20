//
//  OptionPickerViewModel.swift
//  ble_mesh-app
//
//  Created by Michal Lenart on 13/04/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import Foundation

class OptionPickerViewModel {
    let options : [OptionPickerItemViewModel]
    let changeSelectionCallback : (OptionPickerItemViewModel?) -> Void
    let deinitCallback: () -> ()
    
    private(set) var selectedOption : OptionPickerItemViewModel?
    
    init(options:[OptionPickerItemViewModel], selectedOption: OptionPickerItemViewModel?, changeSelectionCallback:@escaping (OptionPickerItemViewModel?) -> Void, onDeinitialization deinitCallback:@escaping () -> ()) {
        self.options = options
        self.selectedOption = selectedOption
        self.changeSelectionCallback = changeSelectionCallback
        self.deinitCallback = deinitCallback
    }
    
    func updateSelection(item: OptionPickerItemViewModel) {
        selectedOption = item
        changeSelectionCallback(item)
    }
    
    func viewWillDisappear() {
        deinitCallback()
    }
}
