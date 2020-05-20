//
//  OptionPickerViewController.swift
//  ble_mesh-app
//
//  Created by Michal Lenart on 13/04/2018.
//  Copyright © 2018 Silicon Labs, http://www.silabs.com All rights reserved.
//

import UIKit

class OptionPickerViewController: UITableViewController {
    
    private weak var emptyMessageLabel: UILabel!
    var viewModel: OptionPickerViewModel?
    var emptyMessageText: String?
    
    override func viewWillAppear(_ animated: Bool) {
        super.viewWillAppear(animated)
        
        setupEmptyMessageLabel()
    }
    
    override func viewWillDisappear(_ animated: Bool) {
        super.viewWillDisappear(animated)
        
        viewModel?.viewWillDisappear()
    }

    override func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return viewModel!.options.count
    }

    override func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCell(withIdentifier: Storyboard.Common.Cell.optionCell, for: indexPath) as! OptionPickerCellView
        let item = viewModel!.options[indexPath.row]
        
        let isCellSelected = item.identifier == viewModel?.selectedOption?.identifier
        
        cell.tickAlpha = isCellSelected ? 1.0 : 0.0
        cell.name = item.name

        return cell
    }
    
    override func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
        tableView.deselectRow(at: indexPath, animated: true)
        
        let item = viewModel!.options[indexPath.row]
        viewModel!.updateSelection(item: item)
        
        for cell in tableView.visibleCells as! [OptionPickerCellView] {
            cell.tickAlpha = 0.0
        }
        
        let cell = tableView.cellForRow(at: indexPath) as! OptionPickerCellView
        cell.tickAlpha = 1.0
    }
    
    override func tableView(_ tableView: UITableView, titleForHeaderInSection section: Int) -> String? {
        return navigationItem.title
    }
    
    private func setupEmptyMessageLabel() {
        guard let emptyMessageText = emptyMessageText else { return }
        
        let noNetworksString = NSAttributedString(string: emptyMessageText, attributes: [ .font : UIFont.systemFont(ofSize: 17, weight: .semibold) ])
        
        let emptyMessageLabel = UILabel(frame: view.bounds)
        emptyMessageLabel.attributedText = noNetworksString
        emptyMessageLabel.textColor = .white
        emptyMessageLabel.numberOfLines = 0
        emptyMessageLabel.textAlignment = .center
        emptyMessageLabel.sizeToFit()
        
        tableView.backgroundView = emptyMessageLabel
        self.emptyMessageLabel = emptyMessageLabel
        self.emptyMessageLabel.isHidden = self.viewModel!.options.count > 0
    }
}
