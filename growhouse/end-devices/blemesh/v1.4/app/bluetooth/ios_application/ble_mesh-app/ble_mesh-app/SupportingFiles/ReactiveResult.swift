//
//  ReactiveResult.swift
//  ble_mesh-app
//
//  Created by Łukasz Rzepka on 01/02/2019.
//  Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
//

import Foundation

struct ReactiveResult<Type> {
    let value: Type
    let error: Error?
}
