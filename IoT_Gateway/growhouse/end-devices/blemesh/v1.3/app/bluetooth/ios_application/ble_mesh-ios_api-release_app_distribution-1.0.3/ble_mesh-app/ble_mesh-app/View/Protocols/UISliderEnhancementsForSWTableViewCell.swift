//
//  UISliderEnhancementsForSWTableViewCell.swift
//  ble_mesh-app
//
//  Created by Piotr Sarna on 05.06.2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import SWTableViewCell

protocol UISliderEnhancementsForSWTableViewCell: class {
    var swScrollView: UIScrollView! { get set }
}

extension UISliderEnhancementsForSWTableViewCell where Self: SWTableViewCell {
    func configureSWScrollView() {
        guard let swScrollView = self.subviews[0] as? UIScrollView else { return }
        
        swScrollView.delaysContentTouches = false
        
        self.swScrollView = swScrollView
    }
    
    func updateSWScrollViewForHit(inView view: UIView?) {
        self.swScrollView.isScrollEnabled = !(view is UISlider)
    }
}

