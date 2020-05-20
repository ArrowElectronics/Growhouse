//
//  Logger.swift
//  ble_mesh-app
//
//  Created by Arkadiusz Marszałek on 21/02/2019.
//  Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
//

import CocoaLumberjack

class Logger {
    
    private init() { }
    
    static func log(message: String, file: String = #file) {
        let fileName = URL(fileURLWithPath: file).deletingPathExtension().lastPathComponent
        DDLogVerbose("Application: \(fileName): \(message)")
    }
}
