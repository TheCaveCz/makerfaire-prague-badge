import Foundation

// super-hacky way to generate list of badges
// save output of this script to `badges.txt`
//
//   swift generate.swift > badges.txt

func isValid(_ num: Int) -> Bool {
    guard ((num & 0b111) != 0) &&
        ((num & 0b111000) != 0) &&
        ((num & 0b111000000) != 0) else { return false }
    
    return (num>>6) != ((num>>3)&0b111) || (num>>6) != (num & 0b111)
}

func tobin(_ num: Int) -> String {
    var res = String(num, radix: 2)
    while (res.count<9) {
        res = "0"+res
    }
    return res
}

func toeeprom(_ num: Int) -> String {
    let res = String(num, radix: 16)
    return "0x\(res[res.index(res.endIndex, offsetBy: -2)..<res.endIndex]),0x\(num<256 ? "00":"01")"
}

let results = (0..<512).filter(isValid(_:))

results.forEach {
    print(isValid($0) ? "\($0)\t\(tobin($0))\t\(toeeprom($0))" : "\($0)")
}
