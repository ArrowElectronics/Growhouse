//
//  RootViewController.swift
//  ble_mesh-app
//
//  Created by Piotr Sarna on 05.06.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import UIKit

protocol SiliconLabsBrandedViewController { }

extension SiliconLabsBrandedViewController where Self: UIViewController {
    func setLogoAsTitleOnNavigationItem() {
        let imageView = UIImageView(frame: CGRect(x: 0, y: 0, width: 53, height: 26))
        imageView.contentMode = .scaleAspectFit
        imageView.image = #imageLiteral(resourceName: "silicon_labs_logo")
        
        let workaroundView = UIView(frame: CGRect(x: 0, y: 0, width: 53, height: 26))
        workaroundView.addSubview(imageView)
        
        self.navigationItem.titleView = workaroundView
    }
}
