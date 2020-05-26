//
//  UpdatableViewModel.swift
//  ble_mesh-app
//
//  Created by Łukasz Rzepka on 30/01/2019.
//  Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
//

import Foundation

protocol UpdatableViewModel {
    func getValuesFromNetwork(completion: (() -> ())?)
}
