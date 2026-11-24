import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { BandejaDeEntradaRespuestasComponent } from './bandeja-de-entrada-respuestas.component';

describe('BandejaDeEntradaRespuestasComponent', () => {
  let component: BandejaDeEntradaRespuestasComponent;
  let fixture: ComponentFixture<BandejaDeEntradaRespuestasComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ BandejaDeEntradaRespuestasComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(BandejaDeEntradaRespuestasComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
