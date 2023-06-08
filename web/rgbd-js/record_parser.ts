import { NativeRecord } from './record';
import { PointerByReference } from './pointer_by_reference';
import { NativeObject } from './native_object';
import { EmscriptenModuleEx } from './emscripten_module_ex';

export class NativeRecordParser extends NativeObject {
  dataPtr: number;

  constructor(module: EmscriptenModuleEx, data: Uint8Array) {
    const parserPtrRef = new PointerByReference(module);
    const dataPtr = module._malloc(data.byteLength);
    module.HEAPU8.set(data, dataPtr);
    const result = module.ccall('rgbd_record_parser_ctor_from_data', 'number', ['number', 'number', 'number'], [parserPtrRef.getPtr(), dataPtr, data.byteLength]);

    const parserPtr = parserPtrRef.getValue();
    parserPtrRef.close();

    if (result < 0)
      throw new Error('Failed to createFileParserFromData');

    const ptr = parserPtr;

    super(module, ptr, true);
    // Need to keep this dataPtr alive to read the data using the parser.
    this.dataPtr = dataPtr;
  }

  delete() {
    this.getModule().ccall('rgbd_record_parser_dtor', null, ['number'], [this.getPtr()]);
    this.getModule()._free(this.dataPtr);
  }

  parse(withFrames: boolean, withDirections: boolean): NativeRecord {
    const filePtr = this.getModule().ccall('rgbd_record_parser_parse',
                                      'number',
                                      ['number', 'boolean', 'boolean'],
                                      [this.getPtr(), withFrames, withDirections]);
    return new NativeRecord(this.getModule(), filePtr);
  }
}
