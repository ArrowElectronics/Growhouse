//
//  OptionPickerViewModel.swift
//  ble_mesh-app
//
//  Created by Michal Lenart on 13/04/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import Foundation

class OptionPickerViewModel {
    
    typealias SelectionCallback = (OptionPickerItemViewModel) -> ()
    typealias DeinitCallback = () -> ()
    
    let options: [OptionPickerItemViewModel]
    let changeSelectionCallback: SelectionCallback?
    let deinitCallback: DeinitCallback?
    
    private(set) var selectedOption: OptionPickerItemViewModel?
    
    init(options: [OptionPickerItemViewModel], selectedOption: OptionPickerItemViewModel?, changeSelectionCallback: SelectionCallback? = nil, onDeinitialization deinitCallback: DeinitCallback? = nil) {
        self.options = options
        self.selectedOption = selectedOption
        self.changeSelectionCallback = changeSelectionCallback
        self.deinitCallback = deinitCallback
    }
    
    func updateSelection(item: OptionPickerItemViewModel) {
        selectedOption = item
        changeSelectionCallback?(item)
    }
    
    func viewWillDisappear() {
        deinitCallback?()
    }
}
