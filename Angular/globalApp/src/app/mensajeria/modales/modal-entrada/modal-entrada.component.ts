import { Component, OnInit, Inject } from '@angular/core';
import { MAT_DIALOG_DATA } from '@angular/material';

@Component({
  selector: 'app-modal-entrada',
  templateUrl: './modal-entrada.component.html',
  styleUrls: ['./modal-entrada.component.css']
})
export class ModalEntradaComponent implements OnInit {

  constructor(@Inject(MAT_DIALOG_DATA) public data: any) {
    // console.log('Data: %o', data);
  }

  ngOnInit() {
  }

}
