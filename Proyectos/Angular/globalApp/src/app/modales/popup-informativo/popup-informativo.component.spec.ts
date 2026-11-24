import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { PopupInformativoComponent } from './popup-informativo.component';

describe('PopupInformativoComponent', () => {
  let component: PopupInformativoComponent;
  let fixture: ComponentFixture<PopupInformativoComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ PopupInformativoComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(PopupInformativoComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
